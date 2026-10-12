// 兼容旧版部署：Manifest.exe（支持 .udf 清单文件参数）
#include "../apps/apps.h"

#include "../common/paths.h"

int main() {
    const std::vector<std::string> args = uw::paths::arguments();
    if (args.size() > 1) {
        return uw::runManifest(uw::paths::absoluteFrom(args[1]));
    }
    return uw::runManifest(std::string());
}
