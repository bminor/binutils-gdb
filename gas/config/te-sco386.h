/* Machine specific defines for the SCO Unix V.3.2 ODT */
#define scounix

/* Local labels start with a period.  */
#define LOCAL_LABEL(name) (name[0] == '.' \
                           && (name[1] == 'L' || name[1] == '.'))
#define FAKE_LABEL_NAME ".L0\001"
/* end of te-sco386.h */
