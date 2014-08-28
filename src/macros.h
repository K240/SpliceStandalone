#ifndef __MACROS_H__
#define __MACROS_H__

#include <FabricCore.h>
#include <FabricSplice.h>

#ifdef FABRIC_SPLICE_TRACE

#define FABRIC_TRY(func, code) \
  try \
  { \
    printf("{--- [%s]\n", func); \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  } \
  catch(FabricSplice::Exception e) \
  { \
    printf("[%s] %s\n", func, e.what()); \
    return; \
  } \
  printf("---} [%s]\n", func);


#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    printf("{--- [%s]\n", func); \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  } \
  catch(FabricSplice::Exception e) \
  { \
    printf("[%s] %s\n", func, e.what()); \
    return returnVal; \
  } \
  printf("---} [%s]\n", func);

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    printf("{--- [%s]\n", func); \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  } \
  catch(FabricSplice::Exception e) \
  { \
  } \
  printf("---} [%s]\n", func);

#else

#define FABRIC_TRY(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  } \
  catch(FabricSplice::Exception e) \
  { \
    printf("[%s] %s\n", func, e.what()); \
    return; \
  }

#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  } \
  catch(FabricSplice::Exception e) \
  { \
    printf("[%s] %s\n", func, e.what()); \
    return returnVal; \
  }

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  } \
  catch(FabricSplice::Exception e) \
  { \
  }

#endif
#endif
