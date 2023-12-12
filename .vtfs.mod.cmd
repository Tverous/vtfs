cmd_/home/tverous/works/vtfs.mod := printf '%s\n'   init.o super.o inode.o dir.o file.o | awk '!x[$$0]++ { print("/home/tverous/works/"$$0) }' > /home/tverous/works/vtfs.mod
