struct _foo
{
  int a[10];
  char *p;
};

class VA
{
 public:
  int va_pub_int;
  char *va_pub_charp;

 private:
  int va_priv_int;
  char *va_priv_charp;

 protected:
  struct _foo bar;
};

class VB
{
 public:
  int vb_pub_int;

  int fvb_pub ();
  virtual int vvb_pub ();

 private:
  int vb_priv_int;
  char *vb_priv_charp;
};

class VC : public VA
{
 public:
  int vc_pub_int;

  int fvc ();
  virtual int vfvc ();
};

class V : public VA, public VB, public VC
{
 public:
  int f ();
  virtual vv ();
  int v_pub_int;
  char *v_pub_charp;

 private:
  int v_priv_int;
  char *v_priv_charp;
};
