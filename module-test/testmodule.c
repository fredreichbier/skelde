#include <object.h>
#include <skstring.h>
#include <bstrlib.h>

void sk_init_module(SkObject *self) {
    sk_object_set_slot(self,
            "from_test",
            sk_string_from_bstring(
                SK_VM,
                bfromcstr(
                    "Huhu from a module!"
                    )
                )
            );
}
