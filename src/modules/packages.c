#include "fastfetch.h"

#include <dirent.h>

#define FF_PACKAGES_MODULE_NAME "Packages"
#define FF_PACKAGES_NUM_FORMAT_ARGS 3

static uint32_t get_num_dirs(const char* dirname) {
    uint32_t num_dirs = 0;
    DIR * dirp;
    struct dirent *entry;

    dirp = opendir(dirname);
    if(dirp == NULL)
        return 0;

    while((entry = readdir(dirp)) != NULL) {
        if(entry->d_type == DT_DIR)
            ++num_dirs;
    }

    num_dirs -= 2; // accounting for . and ..

    closedir(dirp);

    return num_dirs;
}

void ffPrintPackages(FFinstance* instance)
{
    uint32_t pacman = get_num_dirs("/var/lib/pacman/local");
    uint32_t flatpak = get_num_dirs("/var/lib/flatpak/app");

    uint32_t all = pacman + flatpak;

    if(all == 0)
    {
        ffPrintError(instance, FF_PACKAGES_MODULE_NAME, 0, &instance->config.packagesKey, &instance->config.packagesFormat, FF_PACKAGES_NUM_FORMAT_ARGS, "No packages from known package managers found");
        return;
    }

    if(instance->config.packagesFormat.length == 0)
    {
        ffPrintLogoAndKey(instance, FF_PACKAGES_MODULE_NAME, 0, &instance->config.batteryKey);

        #define FF_PRINT_PACKAGE(name) \
        if(name > 0) \
        { \
            printf("%u ("#name")", name); \
            if((all = all - name) > 0) \
                printf(", "); \
        };

        FF_PRINT_PACKAGE(pacman)
        FF_PRINT_PACKAGE(flatpak)

        #undef FF_PRINT_PACKAGE

        putchar('\n');
    }
    else
    {
        ffPrintFormatString(instance, FF_PACKAGES_MODULE_NAME, 0, &instance->config.packagesKey, &instance->config.packagesFormat, NULL, FF_PACKAGES_NUM_FORMAT_ARGS, (FFformatarg[]){
            {FF_FORMAT_ARG_TYPE_UINT, &all},
            {FF_FORMAT_ARG_TYPE_UINT, &pacman},
            {FF_FORMAT_ARG_TYPE_UINT, &flatpak}
        });
    }
}
