/* Functions defined in other files */
extern void extern_func1_1 (int a, char *b, unsigned long c);

/* Functions defined in this file */
static void static_func_1  (int a, char *b, unsigned long c);
static void static_func_2  (int a, char *b, unsigned long c);
static void static_func_3  (int a, char *b, unsigned long c);
static void static_func_4  (int a, char *b, unsigned long c);
static void static_func_5  (int a, char *b, unsigned long c);
static void static_func_6  (int a, char *b, unsigned long c);
static void static_func_7  (int a, char *b, unsigned long c);
static void static_func_8  (int a, char *b, unsigned long c);
static void static_func_9  (int a, char *b, unsigned long c);
static void static_func_10  (int a, char *b, unsigned long c);
static void static_func_11  (int a, char *b, unsigned long c);
static void static_func_12  (int a, char *b, unsigned long c);
static void static_func_13  (int a, char *b, unsigned long c);
static void static_func_14  (int a, char *b, unsigned long c);
static void static_func_15  (int a, char *b, unsigned long c);

void func_1  (int a, char *b, unsigned long c);
void func_2  (int a, char *b, unsigned long c);
void func_3  (int a, char *b, unsigned long c);
void func_4  (int a, char *b, unsigned long c);
void func_5  (int a, char *b, unsigned long c);
void func_6  (int a, char *b, unsigned long c);
void func_7  (int a, char *b, unsigned long c);
void func_8  (int a, char *b, unsigned long c);
void func_9  (int a, char *b, unsigned long c);
void func_10  (int a, char *b, unsigned long c);
void func_11  (int a, char *b, unsigned long c);
void func_12  (int a, char *b, unsigned long c);
void func_13  (int a, char *b, unsigned long c);
void func_14  (int a, char *b, unsigned long c);
void func_15  (int a, char *b, unsigned long c);

int
main (int argc, char *argv[])
{
  int a;
  char *b;
  unsigned long c;

  a = 1;
  b = "This is a string.";
  c = 0xdeadL;

  func_1 (a, b, c);

  exit (0);
}

void
func_1 (int a, char *b, unsigned long c)
{
  func_2 (a, b, c);
}

void
func_2 (int a, char *b, unsigned long c)
{
  func_3 (a, b, c);
}

void
func_3 (int a, char *b, unsigned long c)
{
  func_4 (a, b, c);
}

void
func_4 (int a, char *b, unsigned long c)
{
  func_5 (a, b, c);
}

void
func_5 (int a, char *b, unsigned long c)
{
  func_6 (a, b, c);
}

void
func_6 (int a, char *b, unsigned long c)
{
  func_7 (a, b, c);
}

void
func_7 (int a, char *b, unsigned long c)
{
  func_8 (a, b, c);
}

void
func_8 (int a, char *b, unsigned long c)
{
  func_9 (a, b, c);
}

void
func_9 (int a, char *b, unsigned long c)
{
  func_10 (a, b, c);
}

void
func_10 (int a, char *b, unsigned long c)
{
  func_11 (a, b, c);
}

void
func_11 (int a, char *b, unsigned long c)
{
  func_12 (a, b, c);
}

void
func_12 (int a, char *b, unsigned long c)
{
  func_13 (a, b, c);
}

void
func_13 (int a, char *b, unsigned long c)
{
  func_14 (a, b, c);
}

void
func_14 (int a, char *b, unsigned long c)
{
  func_15 (a, b, c);
}

void
func_15 (int a, char *b, unsigned long c)
{
  static_func_1 (a, b, c);
}

static void
static_func_1 (int a, char *b, unsigned long c)
{
  static_func_2 (a, b, c);
}

static void
static_func_2 (int a, char *b, unsigned long c)
{
  static_func_3 (a, b, c);
}

static void
static_func_3 (int a, char *b, unsigned long c)
{
  static_func_4 (a, b, c);
}

static void
static_func_4 (int a, char *b, unsigned long c)
{
  static_func_5 (a, b, c);
}

static void
static_func_5 (int a, char *b, unsigned long c)
{
  static_func_6 (a, b, c);
}

static void
static_func_6 (int a, char *b, unsigned long c)
{
  static_func_7 (a, b, c);
}

static void
static_func_7 (int a, char *b, unsigned long c)
{
  static_func_8 (a, b, c);
}

static void
static_func_8 (int a, char *b, unsigned long c)
{
  static_func_9 (a, b, c);
}

static void
static_func_9 (int a, char *b, unsigned long c)
{
  static_func_10 (a, b, c);
}

static void
static_func_10 (int a, char *b, unsigned long c)
{
  static_func_11 (a, b, c);
}

static void
static_func_11 (int a, char *b, unsigned long c)
{
  static_func_12 (a, b, c);
}

static void
static_func_12 (int a, char *b, unsigned long c)
{
  static_func_13 (a, b, c);
}

static void
static_func_13 (int a, char *b, unsigned long c)
{
  static_func_14 (a, b, c);
}

static void
static_func_14 (int a, char *b, unsigned long c)
{
  static_func_15 (a, b, c);
}

static void
static_func_15 (int a, char *b, unsigned long c)
{
  extern_func1_1 (a, b, c);
}
