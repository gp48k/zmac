// Output documentation in HTML format or Unix manual page format.

#include <stdio.h>
#include <string.h>

static int html = 1;
static int man = 0;

#ifdef MK_DOC
#define DOC_DEBUG
int main(int argc, char *argv[])
{
	void doc(int pass);

	if (argc > 1 && strcmp(argv[1], "-man") == 0) {
		html = 0;
		man = 1;
	}

	if (man) {
		int i;
		for (i = 0; i < 3; i++)
			doc(i);
	}
	else
		doc(-1);

	return 0;
}
#endif

// Define this to have zmac read from "doc.txt" instead of using the
// internal copy.  It also overwrites the internal copy thus providing
// a way to generate "doc.inl".
//#define DOC_DEBUG

#ifdef DOC_DEBUG
static FILE *fp, *inl;
#define next()	fgetc(fp)
#else
#include "doc.inl"
#define next() *dp++
#endif

static int tt = 0;
static int man_list = 0;
static int man_uppercase = 0;
static int man_nbsp = 0;

static void print(char *str);
static void print_item(char *p);

void doc(int pass)
{
	char line[1024], *p;
	int ch;
	int table = 0, tcol = 0;
	int blockquote = 0;
	int pass_section = 0;

#ifdef DOC_DEBUG
	fp = fopen("doc.txt", "r");
	if (!fp) {
		fprintf(stderr, "Cannot open 'doc.txt'\n");
		return;
	}
	inl = fopen("doc.inl", "w");
	if (!inl) {
		fprintf(stderr, "Cannot write 'doc.inl'\n");
		return;
	}
	fprintf(inl, "static char *documentation =\n");
#else
	char *dp = documentation;
#endif

	for (;;) {
		p = line;
		for (;;) {
			ch = next();
			if (ch <= 0 || ch == '\n')
				break;

			*p++ = ch;
		}
		if (ch <= 0)
			break;

		*p = '\0';

#ifdef DOC_DEBUG
		if (inl) {
			fprintf(inl, "\t\"");
			for (p = line; *p; p++) {
				if (*p == '"' || *p == '\\')
					fprintf(inl, "\\");
				fprintf(inl, "%c", *p);
			}
			fprintf(inl, "\\n\"\n");
		}
#endif

		if (line[0] == '!' && line[1] >= '0' && line[1] <= '9') {
			pass_section = line[1] - '0';
			continue;
		}

		if (pass >= 0 && pass_section != pass)
			continue;

		// Initial '!' is a special hint for manual page output.
		if (*line == '!') {
			if (man) {
				printf("%s\n", line + 1);
				man_list = line[1] && line[2] == 'B';
			}
			continue;
		}

		// In manual page an initial _ will trigger a list item start.
		if (man && man_list && *line == '_') {
			printf(".It ");
		}

		// An initial '|' means part of a block quote
		if (*line == '|') {
			if (html && !blockquote)
				printf("<BLOCKQUOTE>");

			blockquote = 1;
			print(line + 1);
			printf("\n");
			continue;
		}

		if (blockquote) {
			if (html)
				printf("</BLOCKQUOTE>\n");

			blockquote = 0;
		}

		// Start with space means you're a table element.
		// Number of spaces indicate which column.  If not, then we drop
		// out of table mode.  Nothing after the spaces is a special case
		// to remain in table mode.
		if (*line == ' ') {
			int depth = 0;
			for (p = line; *p == ' '; p++)
				depth++;

			if (*p == '\0' && depth == 1)
				continue;

			if (!table) {
				if (html)
					printf("<TABLE>\n");
				else if (man)
					printf(".Bl -tag\n");

				table = 1;
				tcol = 0;
			}

			if (depth < tcol) {
				if (html)
					printf("</TD></TR>\n");
				tcol = 0;
			}

			if (tcol == 0) {
				if (html) {
					printf("<TR><TD VALIGN=\"TOP\">");
					printf("<PRE>"); // Column 1 is fixed width font for us
				}
				if (man) {
					print_item(p);
					printf("\n");
				}

				tcol = 1;
			}
			else if (man && *p) {
				print(p);
				printf("\n");
			}

			while (tcol < depth) {
				if (html) {
					if (tcol == 1)
						printf("</PRE>"); // Column 1 is fixed width font for us
					printf("</TD><TD>");
				}
				tcol++;
			}

			if (html) {
				print(p);
				printf(" ");
			}

			continue;
		}
		else if (table) {
			if (html) {
				if (tcol)
					printf("</TD></TR>\n");

				printf("</TABLE>\n");
			}
			if (man)
				printf(".El\n");

			table = 0;
		}

		// Line starting with '-' generates a full horizontal rule
		if (*line == '-') {
			if (html)
				printf("<HR>\n");
			continue;
		}
		// Empty line denotes paragraph separation
		if (*line == '\0') {
			if (html)
				printf("<P>\n");
			if (man && !man_list)
				printf(".Pp\n");
			continue;
		}
		// Initial . for a heading.  More . for a lesser heading.
		if (*line == '.') {
			for (p = line; *p == '.'; p++)
				;

			if (man) {
				printf(".Sh ");
				man_uppercase = 1;
			}

			if (html)
				printf("<H%d>", 3 + (int)(p - line - 1));

			print(p);

			if (html)
				printf("</H%d>\n", 3 + (int)(p - line - 1));

			if (man)
				printf("\n");

			man_uppercase = 0;

			continue;
		}

		print(line);
		printf("\n");
	}

	if (tt) {
		if (html)
			printf("</TT>\n");
		else if (man)
			printf("\\fP");
	}

	if (html) {
		printf("<p xmlns:dct=\"http://purl.org/dc/terms/\" xmlns:vcard=\"http://www.w3.org/2001/vcard-rdf/3.0#\">\n");
		printf("  <a rel=\"license\"\n");
		printf("     href=\"http://creativecommons.org/publicdomain/zero/1.0/\">\n");
		printf("    <img src=\"http://i.creativecommons.org/p/zero/1.0/88x31.png\" style=\"border-style: none;\" alt=\"CC0\" />\n");
		printf("  </a>\n");
		printf("  <br />\n");
		printf("  To the extent possible under law,\n");
		printf("  <a rel=\"dct:publisher\"\n");
		printf("     href=\"http://48k.ca/zmac.html\">\n");
		printf("    <span property=\"dct:title\">George Phillips</span></a>\n");
		printf("  has waived all copyright and related or neighboring rights to\n");
		printf("  <span property=\"dct:title\">zmac macro cross assembler for the Zilog Z-80 microprocessor</span>.\n");
		printf("This work is published from:\n");
		printf("<span property=\"vcard:Country\" datatype=\"dct:ISO3166\"\n");
		printf("      content=\"CA\" about=\"http://48k.ca/zmac.html\">\n");
		printf("  Canada</span>.\n");
		printf("</p>\n");

		printf("\n");

		printf("<!--\n");
		printf("  If you ran \"zmac --doc\" you may want to send the output\n");
		printf("  to a file using \"zmac --doc >zmac.html\" and then open\n");
		printf("  zmac.html in your web browser.\n");
		printf("-->\n");
	}

#ifdef DOC_DEBUG
	fclose(fp);
	if (inl) {
		fprintf(inl, ";\n");
		fclose(inl);
	}
#endif
}

static void link(char *str, char how)
{
	char *id = strchr(str, how);
	char *text = id, save, id_save, *id_end;
	int len = 0, spccnt, full_url;

	while (id[len] == how)
		len++;

	for (id_end = id + len; *id_end >= 'a' && *id_end <= 'z'; id_end++)
		;

	// Allow for @http: full URL links
	if (*id_end == ':') {
		for (; *id_end && *id_end != ' ' && *id_end != '\t'; id_end++)
			;
	}

	spccnt = len;
	do {
		if (text > str)
			text--;

		while (text > str && *text != ' ')
			text--;

		spccnt--;
	}
	while (spccnt > 0);

	if (*text == ' ')
		text++;

	save = *text;
	*text = '\0';
	print(str);
	*text = save;

	save = *id_end;
	*id_end = '\0';

	full_url = strchr(id + len, ':') ? 1 : 0;

	if (html) {
		printf("<A ");
		if (how == '@')
			printf("HREF=\"%s", full_url ? "" : "#");
		else
			printf("NAME=\"");
		printf("%s\">", id + len);
	}

	id_save = *id;
	*id = '\0';
	print(text);
	*id = id_save;

	if (man && full_url)
		printf(" [%s] ", id + len);

	*id_end = save;

	if (html)
		printf("</A>");

	print(id_end);
}

// Output string with HTML escaping and certain internal metacharacter handling.

static void print(char *str)
{
	// Check for @id (link-to) and \id (link-target) specifiers.
	char *to = strchr(str, '@');
	char *target = strchr(str, '\\');

	// Hackily disallow _@ to trigger link generation.
	if (to && to > str && to[-1] == '_')
		to = 0;

	if (to && !((to[1] >= 'a' && to[1] <= 'z') || to[1] == '@'))
		to = 0;

	if (target && !((target[1] >= 'a' && target[1] <= 'z') || target[1] == '@'))
		target = 0;

	if (to && target && target < to)
		to = 0;

	if (to && target && to < target)
		target = 0;

	if (to) {
		link(str, '@');
		return;
	}
	else if (target) {
		link(str, '\\');
		return;
	}

	while (*str) {
		// Two or more ## mean N-1 non-breaking spaces.
		if (str[0] == '#' && str[1] == '#') {
			int n = 0;
			for (; *str == '#'; str++)
				n++;

			while (n-- > 1) {
				if (html)
					printf("&nbsp;");
				if (man)
					printf("\\ ");
			}

			continue;
		}

		if (*str == '_') {
			if (str[1] == '_') {
				printf("_");
				str++;
			}
			else {
				if (html)
					printf("<%sTT>", tt ? "/" : "");
				else if (man)
					printf("\\f%s", tt ? "P" : "C");

				tt = !tt;
			}
		}
		else if (html && *str == '<')
			printf("&lt;");
		else if (html && *str == '>')
			printf("&gt;");
		else if (html && *str == '&')
			printf("&amp;");
		else if (man && man_uppercase && *str >= 'a' && *str <= 'z')
			printf("%c", *str - 'a' + 'A');
		else if (man && man_nbsp && *str == ' ')
			printf("\\ ");
		else if (man && *str == '\\')
			printf("\\\\");
		else
			printf("%c", *str);

		str++;
	}
}

static void print_item(char *p)
{
	// Bit of of guessing to figure out what kind of item this is.
	if (*p == '-') {
		printf(".It Fl ");
		p++;
		if (*p == '-')
			printf("\\");

		while (*p != '\0' && *p != ' ' && *p != '\t')
			printf("%c", *p++);

		if (*p) {
			printf(" Ar ");
			print(p);
		}
	}
	else {
		printf(".It ");
		man_nbsp = 1;
		print(p);
		man_nbsp = 0;
	}
}
