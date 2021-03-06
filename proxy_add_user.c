/*
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                   Version 2, December 2004
 *
 * Copyright (C) 2010 Sebastien Rannou
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <apache2/httpd.h>
#include <apache2/http_config.h>

#include <stdio.h>

/*
 * Module written by sebastien rannou <mxs@buffout.org>
 *
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
 */

module AP_MODULE_DECLARE_DATA proxy_add_user_module;

/* Configuration settings */
typedef struct
{
  int		enabled;	/* != 0 if enabled */
  const char	*key_name;	/* default to X-REMOTE_USER */
} proxy_add_user_config;

/* allocated a new configuration */
static void *
proxy_add_user_create_config(apr_pool_t *pool, char *s)
{
  proxy_add_user_config	*cfg;

  if ((cfg = apr_pcalloc(pool, sizeof(*cfg))) != NULL)
    {
      cfg->key_name = "X-REMOTE_USER";
    }

  return cfg;
}

/* Enable (or not) the module */
static const char *
proxy_add_user_enable(cmd_parms *cmd, void *config, const char *arg)
{
  proxy_add_user_config	*cfg = (proxy_add_user_config *) config;

  if (strcasecmp(arg, "On") == 0)
    {
      cfg->enabled = 1;
      return NULL;
    }
  if (strcasecmp(arg, "Off") == 0)
    {
      cfg->enabled = 0;
      return NULL;
    }
  return "ProxyAddUser must be set to \"On\" or \"Off\"";
}

/* Change the header key to the value pointed by arg */
static const char *
proxy_add_user_set_key(cmd_parms *cmd, void *config, const char *arg)
{
  proxy_add_user_config	*cfg = (proxy_add_user_config *) config;

  cfg->key_name = arg;
  return NULL;
}

/* Configuration settings */
static const command_rec 
proxy_add_user_commands[] =
  {
    /* Enable or not the module */
    AP_INIT_TAKE1("ProxyAddUser", 
		  proxy_add_user_enable, 
		  NULL, 
		  OR_AUTHCFG,
		  "Enable ProxyAddUser <On | Off>"),
    /* Which key to use */
    AP_INIT_TAKE1("ProxyAddUserKey",
		  proxy_add_user_set_key,
		  NULL,
		  OR_AUTHCFG,
		  "Header key to be set (default is X-REMOTE_USER)"),
    {NULL}
  };

/* If REMOTE_USER is available, add it to headers */
static int
proxy_add_user_handler(request_rec *request)
{
  void			*cfg;

  cfg = ap_get_module_config(request->per_dir_config, &proxy_add_user_module);
  if (cfg && ((proxy_add_user_config *) cfg)->enabled && request->user != NULL)
    {
      apr_table_set(request->headers_in, 
		    ((proxy_add_user_config *) cfg)->key_name, request->user);
      apr_table_unset(request->headers_in, "Authorization");
    }
  return DECLINED;
}

/* Register hooks */
static void
proxy_add_user_register_hooks(apr_pool_t *pool)
{
  /* only one hook called before performing the proxy request */
  ap_hook_handler(proxy_add_user_handler, NULL, NULL, APR_HOOK_FIRST);
}

/* Register the module */
module AP_MODULE_DECLARE_DATA
proxy_add_user_module =
  {
    STANDARD20_MODULE_STUFF,
    proxy_add_user_create_config,
    NULL,
    NULL,
    NULL,
    proxy_add_user_commands,
    proxy_add_user_register_hooks
  };
