#include "assert.h"
#include "stdint.h"
#include "stdio.h"

inline void assert_failed_(uint8_t *file, uint32_t line)
{
#ifdef USE_DEVLIB_ASSERT
    int _line = (int)line;
    printf("Wrong parameters value: file %s on line %d\r\n", file, _line);
#else
    while (0)
        ;
#endif
}

void assert_failed(uint8_t *file, uint32_t line)
{
    int _line = (int)line;
    printf("Wrong parameters value: file %s on line %d\r\n", file, _line);
}
