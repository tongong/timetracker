PREFIX := /usr/local

CC := gcc

timetracker-save: timetracker-save.c
	${CC} -o $@ $<

clean:
	rm -f timetracker-save

install: timetracker-save
	install -D -m755 timetracker ${DESTDIR}${PREFIX}/bin/timetracker
	install -D -m755 timetracker-save ${DESTDIR}${PREFIX}/bin/timetracker-save
	install -D -m755 timetracker-report ${DESTDIR}${PREFIX}/bin/timetracker-report

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/timetracker
	rm -f ${DESTDIR}${PREFIX}/bin/timetracker-save
	rm -f ${DESTDIR}${PREFIX}/bin/timetracker-report

.PHONY: install uninstall
