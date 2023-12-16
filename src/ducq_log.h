#ifndef _DUCQ_LOG_HEADER_
#define _DUCQ_LOG_HEADER_


#define DUCQ_LOG_ROUTE "LOG"

enum ducq_log_level {
	DUCQ_LOG_DEBUG,
	DUCQ_LOG_INFO,
	DUCQ_LOG_WARNING,
	DUCQ_LOG_ERROR
};
char *ducq_level_tostr(enum ducq_log_level level);

#define DUCQ_TIMESTAMP_SIZE sizeof("YYYY-MM-DDTHH:MM:SS")
char *ducq_getnow(char *buffer, size_t size);


#define RESET		"\033[0m"

#define BOLD		"\033[1m"
#define ITALIC		"\033[3m"
#define UNDERLINE	"\033[4m"
#define BLINK		"\033[5m"
#define INVERSE		"\033[7m"
#define CONCEAL		"\033[8m"
#define STRIKE		"\033[9m"

#define BOLD_OFF	"\033[21m"
#define ITALIC_OFF	"\033[23m"
#define UNDERLINE_OFF	"\033[24m"
#define BLINK_OFF	"\033[25m"
#define INVERSE_OFF	"\033[27m"
#define CONCEAL_OFF	"\033[28m"
#define STRIKE_OFF	"\033[29m"

#define FG_NORMAL	"\033[39m"
#define FG_DARK_BLACK	"\033[30m"
#define FG_DARK_RED	"\033[31m"
#define FG_DARK_GREEN	"\033[32m"
#define FG_DARK_YELLOW	"\033[33m"
#define FG_DARK_BLUE  	"\033[34m"
#define FG_DARK_MAGENTA	"\033[35m"
#define FG_DARK_CYAN	"\033[36m"
#define FG_DARK_WHITE	"\033[37m"
#define FG_LITE_BLACK	"\033[90m"
#define FG_LITE_RED	"\033[91m"
#define FG_LITE_GREEN	"\033[92m"
#define FG_LITE_YELLOW	"\033[93m"
#define FG_LITE_BLUE  	"\033[94m"
#define FG_LITE_MAGENTA	"\033[95m"
#define FG_LITE_CYAN	"\033[96m"
#define FG_LITE_WHITE	"\033[97m"

#define BG_NORMAL	"\033[49m"
#define BG_DARK_BLACK	"\033[40m"
#define BG_DARK_RED	"\033[41m"
#define BG_DARK_GREEN	"\033[42m"
#define BG_DARK_YELLOW	"\033[43m"
#define BG_DARK_BLUE  	"\033[44m"
#define BG_DARK_MAGENTA	"\033[45m"
#define BG_DARK_CYAN	"\033[46m"
#define BG_DARK_WHITE	"\033[47m"
#define BG_LITE_BLACK	"\033[100m"
#define BG_LITE_RED	"\033[101m"
#define BG_LITE_GREEN	"\033[102m"
#define BG_LITE_YELLOW	"\033[103m"
#define BG_LITE_BLUE  	"\033[104m"
#define BG_LITE_MAGENTA	"\033[105m"
#define BG_LITE_CYAN	"\033[106m"
#define BG_LITE_WHITE	"\033[107m"



#endif // _DUCQ_LOG_HEADER_

