#include "init_signal.h"
#include <signal.h>

void init_signal()
{
	signal(SIGPIPE, SIG_IGN);
}
