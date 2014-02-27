This is unix grep program analog.

To build:
	make

To run:
	./grep [-Rc1] TEXT FILE1 [...]

Availible options:
	- R -- recursive (need for catalogs)
	- c -- enable color output (enabled automatically, if you use terminal (emulator))
	- 1 -- short output. Print 40 characters maximum after text find

To clear:
	make clear
