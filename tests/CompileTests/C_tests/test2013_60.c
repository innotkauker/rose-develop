
int handle_sigchld()
   {
     int status;
     int pid;
     char signame[32];

  // DQ (9/5/2013): It is a problem when the statement expression is in a function paramter.
  // if (sig2str(((union {__pid_t __in;int __i;}){status} . __i & 0xff00) >> 8,signame) == -1)
     if (sig2str(((union {int __in;int __i;}){status} . __i & 0xff00) >> 8,signame) == -1)
        {
          signame[0] = 'x';
        }
   }

