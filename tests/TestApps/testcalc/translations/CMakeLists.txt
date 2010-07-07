add_custom_command(OUTPUT testcalc.ts testcalc.qm
  COMMAND lupdate
  ARGS -locations absolute -no-ui-lines -no-sort
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  -ts ${CMAKE_CURRENT_SOURCE_DIR}/testcalc.ts

  COMMAND lrelease
  ARGS -idbased -markuntranslated "!!\\"
  ${CMAKE_CURRENT_SOURCE_DIR}/testcalc.ts
  -qm ${CMAKE_CURRENT_SOURCE_DIR}/testcalc.qm
        
  VERBATIM
)

add_custom_target(translation_target ALL
  DEPENDS testcalc.ts testcalc.qm
)
