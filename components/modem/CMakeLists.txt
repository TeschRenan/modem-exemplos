set(srcs "src/modem.cpp")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    REQUIRES driver)