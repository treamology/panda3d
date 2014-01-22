set(P3PSTATCLIENT_HEADERS
  config_pstats.h pStatClient.I pStatClient.h
  pStatClientImpl.I pStatClientImpl.h
  pStatClientVersion.I 
  pStatClientVersion.h pStatClientControlMessage.h 
  pStatCollector.I pStatCollector.h pStatCollectorDef.h 
  pStatCollectorForward.I pStatCollectorForward.h
  pStatFrameData.I pStatFrameData.h pStatProperties.h 
  pStatServerControlMessage.h pStatThread.I pStatThread.h 
  pStatTimer.I pStatTimer.h
)

set(P3PSTATCLIENT_SOURCES
  config_pstats.cxx pStatClient.cxx pStatClientImpl.cxx
  pStatClientVersion.cxx 
  pStatClientControlMessage.cxx
  pStatCollector.cxx
  pStatCollectorDef.cxx 
  pStatCollectorForward.cxx
  pStatFrameData.cxx pStatProperties.cxx 
  pStatServerControlMessage.cxx
  pStatThread.cxx
)

composite_sources(p3pstatclient P3PSTATCLIENT_SOURCES)
add_library(p3pstatclient ${P3PSTATCLIENT_HEADERS} ${P3PSTATCLIENT_SOURCES})
target_link_libraries(p3pstatclient p3putil)
target_interrogate(p3pstatclient ALL)

#begin test_bin_target
  #define LOCAL_LIBS \
   # p3pstatclient 
  #define OTHER_LIBS \
  #  $[OTHER_LIBS] p3pystub

  #define TARGET test_client

  #define SOURCES \
  #  test_client.cxx

#end test_bin_target