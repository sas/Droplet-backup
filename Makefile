SUBDIRS	= src

all:
	@for dir in $(SUBDIRS); do make -C $$dir; done

clean:
	@for dir in $(SUBDIRS); do make -C $$dir clean; done

distclean:
	@for dir in $(SUBDIRS); do make -C $$dir distclean; done
