set(srcs "main.c"
	"src/sht1x.c"
	"src/sht1x-puertos.c"
)

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")




