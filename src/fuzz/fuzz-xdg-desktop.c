/* SPDX-License-Identifier: LGPL-2.1+ */

#include "xdg-autostart-service.h"
#include "alloc-util.h"
#include "string-util.h"
#include "tmpfile-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "rm-rf.h"
#include "strv.h"
#include "tests.h"
#include "fuzz.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
        _cleanup_(unlink_tempfilep) char name[] = "/tmp/fuzz-xdg-desktop.XXXXXX";
        _cleanup_close_ int fd = -1;
        _cleanup_(xdg_autostart_service_freep) XdgAutostartService *service = NULL;
        _cleanup_(rm_rf_physical_and_freep) char *tmpdir = NULL;

        /* We don't want to fill the logs with messages about parse errors.
         * Disable most logging if not running standalone */
        if (!getenv("SYSTEMD_LOG_LEVEL"))
                log_set_max_level(LOG_CRIT);

        assert_se(mkdtemp_malloc("/tmp/fuzz-xdg-desktop-XXXXXX", &tmpdir) >= 0);

        fd = mkostemp_safe(name);
        assert_se(fd >= 0);
        assert_se(write(fd, data, size) == (ssize_t) size);

        service = xdg_autostart_service_parse_desktop(name);
        service->name = strdup("fuzz-xdg-desktop.service");
        if (service)
                (void) xdg_autostart_service_generate_unit(service, tmpdir);

        return 0;
}
