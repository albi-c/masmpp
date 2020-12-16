#include "clipboard.hpp"

std::string clipboardRead() {
    FILE* fp = popen("xclip -o -selection clipboard", "r");
    if (!fp) {
        log::e("Failed to start xclip to read clipboard");
        return "";
    }
    
    std::string sbuf;
    char buf[1024];
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        sbuf += std::string(buf);
    }
    pclose(fp);
    return sbuf;
}

void clipboardWrite(std::string text) {
    FILE* fp = popen("xclip -i -selection clipboard", "w");
    if (!fp) {
        log::e("Failed to start xclip to write clipboard");
        return;
    }

    fprintf(fp, "%s\n", text.c_str());
    pclose(fp);
}
