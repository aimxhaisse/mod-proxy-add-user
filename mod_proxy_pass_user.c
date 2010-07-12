#include "httpd.h"
#include "http_config.h"

#include <stdio.h>

/*
 * A tiny module to pass REMOTER_USER environment variable to proxified
 * applications through http headers.
 *
 * Apache's documentation recommends using "LA-U:REMOTE_USER" to solve this 
 * problematic, however this is sometimes not possible because
 * some modules set it in the handler hook, too bad :(
 *
 * The module itself is ugly, because it depends on modules that set environment
 * variable into the handler hook, before declining, instead of doing this in the
 * fixup phase. So it registers just after these hooks, before the proxy handler.
 *
 * That's why the module MUST be loaded between your authentication module and
 * the proxy module, so as to be called at the right moment. This is really ugly,
 * but actually the only solution with some authentication modules.
 *
 * @todo: add configuration directives
 */

/* If REMOTE_USER is available, add it to headers */
static int mod_proxy_pass_user_handler(request_rec *request)
{
  if (request->user != NULL)
    {
      apr_table_set(request->headers_in, "X-REMOTE_USER", request->user);
      apr_table_unset(request->headers_in, "Authorization");
    }
  return DECLINED;
}

/* Register hooks */
static void
mod_proxy_pass_user_register_hooks(apr_pool_t *pool)
{
  /* only one hook called before performing the proxy request */
  ap_hook_handler(mod_proxy_pass_user_handler, NULL, NULL, APR_HOOK_FIRST);
}

/* Register the module */
module AP_MODULE_DECLARE_DATA mod_proxy_pass_user_module =
  {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &mod_proxy_pass_user_register_hooks
  };
