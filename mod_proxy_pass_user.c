#include "httpd.h"
#include "http_config.h"

/*
 * A tiny module to pass REMOTER_USER environment variable to proxified
 * applications through http headers.
 *
 * Apache's documentation recommends using "LA-U:REMOTE_USER" to solve this 
 * problematic, however this sometimes is not possible (mod_auth_openid).
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
}

/* Register hooks */
static void
mod_proxy_pass_user_module(apr_pool_t *pool)
{
  /* only one hook called before performing the proxy request */
  ap_hook_fixups(mod_proxy_pass_user_handler, NULL, NULL, APR_HOOK_LAST);
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
