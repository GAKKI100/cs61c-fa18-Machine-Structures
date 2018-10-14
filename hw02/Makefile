# Your program must compile with 'make'
# You must not change this file.

CC = gcc
FLAGS = -std=c99 -O0 -Wall -Werror -g -pedantic

default:
	$(CC) $(FLAGS) rgrep.c matcher.c -o rgrep

clean:
	rm -f rgrep
	rm -rf *.dSYM

check: clean default
	test "`echo "a\nb\nc" | ./rgrep 'a'`" = "a"
	test "`echo "a\n" | ./rgrep 'a'`" = "a"
	test "`echo "a" | ./rgrep '...'`" = ""
	test "`echo "abc" | ./rgrep '.b.'`" = "abc"
	test "`echo "h\naaaaah" | ./rgrep 'a+h'`" = "aaaaah"
	test "`echo "h\naaaaahhhhh" | ./rgrep 'aa+hh+'`" = "aaaaahhhhh"
	test "`echo "a" | ./rgrep 'a?a'`" = "a"
	test "`echo "aa" | ./rgrep 'a?a'`" = "aa"
	test "`echo "woot\nwot\nwat\n" | ./rgrep 'wo?t'`" = "wot"
	test "`echo "CCCCCCC\nC+\nC++" | ./rgrep '.\+\+'`" = "C++"
	test "`echo "GG" | ./rgrep '.+'`" = "GG"
	test "`echo "Boraaaaaaaaaaaa" | ./rgrep 'Bora?.+'`" = "Boraaaaaaaaaaaa"
	test "`echo "nicknicknic" | ./rgrep '.+k'`" = "nicknicknic"
	test "`echo "jo" | ./rgrep 'jon+'`" = ""
	test "`echo "jonMurataaaa" | ./rgrep 'jonMurata+a'`" = "jonMurataaaa"
	test "`echo "dan garcia" | ./rgrep 'dan ga+arcia'`" = ""
	test "`echo "Sruthi" | ./rgrep 'Sru\.hi'`" = ""
	test "`echo "61C homework...\n...can take a long time" | ./rgrep 'work\.+'`" = "61C homework..."
	@echo "Passed sanity check."

	
