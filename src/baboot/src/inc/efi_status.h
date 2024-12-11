#ifndef EFI_STATUS_H
#define EFI_STATUS_H

// Helping macro functions
#define EFI_ERROR(a) (a < 0)
#define EFIERR(a)    (EFI_STATUS)(0x8000000000000000 | a)
#define ERRCODE(a)   (a & 0b0111111111111111111111111111111111111111111111111111111111111111)

// EFI_STATUS Values
#define EFI_SUCCESS                      0
#define EFI_LOAD_ERROR            EFIERR(1)
#define EFI_INVALID_PARAMETER     EFIERR(2)
#define EFI_UNSUPPORTED           EFIERR(3)
#define EFI_BUFFER_TOO_SMALL      EFIERR(5)
#define EFI_NOT_FOUND             EFIERR(14)
#define EFI_NOT_STARTED           EFIERR(19)
#define EFI_INCOMPATIBLE_VERSION  EFIERR(25)

#endif
