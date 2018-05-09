all: subdirs

.PHONY: all subdirs clean
subdirs:
	${MAKE} -C X11        ${MAKECMDGOALS}
	${MAKE} -C png        ${MAKECMDGOALS}
	${MAKE} -C PS         ${MAKECMDGOALS}
	${MAKE} -C projection ${MAKECMDGOALS}

clean: subdirs


