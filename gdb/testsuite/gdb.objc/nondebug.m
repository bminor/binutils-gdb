#include <stdio.h>
#include <Foundation/NSObject.h>

@interface NonDebug: NSObject
{
}
@end
@interface NonDebug2: NSObject
{
}
@end

@implementation NonDebug

- someMethod
{
  printf("method someMethod\n");
  return self;
}

@end
@implementation NonDebug2

- someMethod
{
  printf("method2 someMethod\n");
  return self;
}

@end


int main (int argc, const char *argv[])
{
  id obj;
  obj = [NonDebug new];
  [obj someMethod];
  return 0;
}
