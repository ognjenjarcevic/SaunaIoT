# To "make"       run in terminal: "make node" or "make monitor"
# To "make clean" run in terminal: "make node_clean" or "make monitor_clean"
node:
	make -f makefile.node
node_clean:
	make clean -f makefile.node
monitor:
	make -f makefile.monitor
monitor_clean:
	make clean -f makefile.monitor
