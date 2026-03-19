#ifndef _SCRIPT_DUMP_H_
#define _SCRIPT_DUMP_H_
struct config_dump{
	const char *script, *target;
	const struct reader_driver *reader;
	long mappernum;
	long submappernum;
	//struct romimage rom;
	struct {long cpu, ppu;} increase;
	bool progress;
};
void script_dump_execute(struct config_dump *c);
#endif
