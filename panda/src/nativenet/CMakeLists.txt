if(NOT WANT_NATIVE_NET)
  return()
endif()

set(P3NATIVENET_HEADERS
  config_nativenet.h
  buffered_datagramconnection.h
  buffered_datagramreader.h buffered_datagramreader.I
  ringbuffer.h ringbuffer.I socket_ip.h
  socket_tcp.h socket_tcp_listen.h socket_tcp_ssl.h
  time_accumulator.h time_out.h
  socket_address.I socket_address.h
  socket_portable.h  time_base.h time_span.h buffered_datagramwriter.h
  socket_base.h socket_selector.h
  socket_udp.h
  socket_udp_incoming.h time_clock.h
  membuffer.h membuffer.I socket_fdset.h
  socket_udp_outgoing.h time_general.h
)

set(P3NATIVENET_SOURCES
  config_nativenet.cxx
  buffered_datagramconnection.cxx
  socket_address.cxx
  socket_ip.cxx
  socket_tcp.cxx
  socket_tcp_listen.cxx
  socket_tcp_ssl.cxx
  socket_udp.cxx
  socket_udp_incoming.cxx
  socket_udp_outgoing.cxx
)

composite_sources(p3nativenet P3NATIVENET_SOURCES)
add_component_library(p3nativenet SYMBOL BUILDING_PANDA_NATIVENET
  ${P3NATIVENET_HEADERS} ${P3NATIVENET_SOURCES})
target_link_libraries(p3nativenet p3pandabase pandaexpress)
target_interrogate(p3nativenet ALL)

if(NOT BUILD_METALIBS)
  install(TARGETS p3nativenet
    EXPORT Core COMPONENT Core
    DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE COMPONENT CoreDevel)
endif()
install(FILES ${P3NATIVENET_HEADERS} COMPONENT CoreDevel DESTINATION include/panda3d)
