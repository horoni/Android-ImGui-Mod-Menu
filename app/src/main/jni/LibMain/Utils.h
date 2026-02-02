// taken from here https://github.com/reveny/Android-ImGui-Mod-Menu/blob/main/app/src/main/jni/Loader/Loader.cpp
std::string GetNativeLibraryDirectory(/* const char *library*/) {
    char buffer[512];
    FILE *fp = fopen("/proc/self/maps", "re");
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, "libmain.so")) {
                RemapTools::ProcMapInfo info{};
                char perms[10];
                char path[255];
                char dev[25];

                sscanf(buffer, "%lx-%lx %s %ld %s %ld %s", &info.start, &info.end, perms, &info.offset, dev, &info.inode, path);
                info.path = path;

                //Remove libmain.so to get path
                std::string pathStr = std::string(info.path);
                if (!pathStr.empty()) {
                    pathStr.resize(pathStr.size() - 10); //libmain.so = 10 chars
                }

                return pathStr;
            }
        }
    }
    return "";
}
const char *GetPackageName() {
    //https://stackoverflow.com/questions/42918762/how-to-get-app-package-name-or-applicationid-using-jni-android
    static char *application_id[256];
    FILE *fp = fopen("/proc/self/cmdline", "r");
    if (fp) {
        fread(application_id, sizeof(application_id), 1, fp);
        fclose(fp);
    }
    return (const char *) application_id;
}