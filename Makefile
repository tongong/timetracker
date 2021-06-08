PREFIX := /usr/local

CC := gcc

timetracker-save: timetracker-save.c
	${CC} -o $@ $<

clean:
	rm -f timetracker-save

install: timetracker-save
	install -D -m755 timetracker ${DESTDIR}${PREFIX}/bin/timetracker
	install -D -m755 timetracker-save ${DESTDIR}${PREFIX}/bin/timetracker-save

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/timetracker ${DESTDIR}${PREFIX}/bin/timetracker-save

.PHONY: install uninstall
