# To "make"       run in terminal: "make node" or "make monitor" or "make user"
# To "make clean" run in terminal: "make node_clean" or "make monitor_clean" or "make user_clean"
node:
	make -f makefile.node
node_clean:
	make clean -f makefile.node
monitor:
	make -f makefile.monitor
monitor_clean:
	make clean -f makefile.monitor
user:
	make -f makefile.user
user_clean:
	make clean -f makefile.user
