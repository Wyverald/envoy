1.21.0 (Pending)
================

Incompatible Behavior Changes
-----------------------------
*Changes that are expected to cause an incompatibility if applicable; deployment changes are likely required*

* xds: ``*`` became a reserved name for a wildcard resource that can be subscribed to and unsubscribed from at any time. This is a requirement for implementing the on-demand xDSes (like on-demand CDS) that can subscribe to specific resources next to their wildcard subscription. If such xDS is subscribed to both wildcard resource and to other specific resource, then in stream reconnection scenario, the xDS will not send an empty initial request, but a request containing ``*`` for wildcard subscription and the rest of the resources the xDS is subscribed to. If the xDS is only subscribed to wildcard resource, it will try to send a legacy wildcard request. This behavior implements the recent changes in :ref:`xDS protocol <xds_protocol>` and can be temporarily reverted by setting the ``envoy.restart_features.explicit_wildcard_resource`` runtime guard to false.

Minor Behavior Changes
----------------------
*Changes that may cause incompatibilities for some users, but should not for most*

* config: the log message for "gRPC config stream closed" now uses the most recent error message, and reports seconds instead of milliseconds for how long the most recent status has been received.
* dns: now respecting the returned DNS TTL for resolved hosts, rather than always relying on the hard-coded :ref:`dns_refresh_rate. <envoy_v3_api_field_config.cluster.v3.Cluster.dns_refresh_rate>` This behavior can be temporarily reverted by setting the runtime guard ``envoy.reloadable_features.use_dns_ttl`` to false.
* listener: destroy per network filter chain stats when a network filter chain is removed during the listener in place update.

Bug Fixes
---------
*Changes expected to improve the state of the world and are unlikely to have negative effects*

* listener: fixed the crash when updating listeners that do not bind to port.
* thrift_proxy: fix the thrift_proxy connection manager to correctly report success/error response metrics when performing :ref:`payload passthrough <envoy_v3_api_field_extensions.filters.network.thrift_proxy.v3.ThriftProxy.payload_passthrough>`.

Removed Config or Runtime
-------------------------
*Normally occurs at the end of the* :ref:`deprecation period <deprecated>`

* compression: removed ``envoy.reloadable_features.enable_compression_without_content_length_header`` runtime guard and legacy code paths.
* health check: removed ``envoy.reloadable_features.health_check.immediate_failure_exclude_from_cluster`` runtime guard and legacy code paths.
* http: removed ``envoy.reloadable_features.add_and_validate_scheme_header`` and legacy code paths.
* http: removed ``envoy.reloadable_features.check_unsupported_typed_per_filter_config``, Envoy will always check unsupported typed per filter config if the filter isn't optional.
* http: removed ``envoy.reloadable_features.dont_add_content_length_for_bodiless_requests deprecation`` and legacy code paths.
* http: removed ``envoy.reloadable_features.http2_skip_encoding_empty_trailers`` and legacy code paths. Envoy will always encode empty trailers by sending empty data with ``end_stream`` true (instead of sending empty trailers) for HTTP/2.
* http: removed ``envoy.reloadable_features.improved_stream_limit_handling`` and legacy code paths.
* http: removed ``envoy.reloadable_features.remove_forked_chromium_url`` and legacy code paths.
* http: removed ``envoy.reloadable_features.return_502_for_upstream_protocol_errors``. Envoy will always return 502 code upon encountering upstream protocol error.
* http: removed ``envoy.reloadable_features.treat_host_like_authority`` and legacy code paths.
* http: removed ``envoy.reloadable_features.treat_upstream_connect_timeout_as_connect_failure`` and legacy code paths.
* upstream: removed ``envoy.reloadable_features.upstream_host_weight_change_causes_rebuild`` and legacy code paths.

New Features
------------
* api: added support for *xds.type.v3.TypedStruct* in addition to the now-deprecated *udpa.type.v1.TypedStruct* proto message, which is a wrapper proto used to encode typed JSON data in a *google.protobuf.Any* field.
* bootstrap: added :ref:`typed_dns_resolver_config <envoy_v3_api_field_config.bootstrap.v3.Bootstrap.typed_dns_resolver_config>` in the bootstrap to support DNS resolver as an extension.
* cluster: added :ref:`typed_dns_resolver_config <envoy_v3_api_field_config.cluster.v3.Cluster.typed_dns_resolver_config>` in the cluster to support DNS resolver as an extension.
* config: added :ref:`environment_variable <envoy_v3_api_field_config.core.v3.datasource.environment_variable>` to the :ref:`DataSource <envoy_v3_api_msg_config.core.v3.datasource>`.
* dns_cache: added :ref:`typed_dns_resolver_config <envoy_v3_api_field_extensions.common.dynamic_forward_proxy.v3.DnsCacheConfig.typed_dns_resolver_config>` in the dns_cache to support DNS resolver as an extension.
* dns_filter: added :ref:`typed_dns_resolver_config <envoy_v3_api_field_extensions.filters.udp.dns_filter.v3.DnsFilterConfig.ClientContextConfig.typed_dns_resolver_config>` in the dns_filter to support DNS resolver as an extension.
* dns_resolver: added :ref:`CaresDnsResolverConfig<envoy_v3_api_msg_extensions.network.dns_resolver.cares.v3.CaresDnsResolverConfig>` to support c-ares DNS resolver as an extension.
* dns_resolver: added :ref:`AppleDnsResolverConfig<envoy_v3_api_msg_extensions.network.dns_resolver.apple.v3.AppleDnsResolverConfig>` to support apple DNS resolver as an extension.
* ext_authz: added :ref:`query_parameters_to_set <envoy_v3_api_field_service.auth.v3.OkHttpResponse.query_parameters_to_set>` and :ref:`query_parameters_to_remove <envoy_v3_api_field_service.auth.v3.OkHttpResponse.query_parameters_to_remove>` for adding and removing query string parameters when using a gRPC authorization server.
* http: added support for :ref:`retriable health check status codes <envoy_v3_api_field_config.core.v3.HealthCheck.HttpHealthCheck.retriable_statuses>`.
* oauth filter: added :ref:`cookie_names <envoy_v3_api_field_extensions.filters.http.oauth2.v3.OAuth2Credentials.cookie_names>` to allow overriding (default) cookie names (``BearerToken``, ``OauthHMAC``, and ``OauthExpires``) set by the filter.
* thrift_proxy: add upstream response zone metrics in the form ``cluster.cluster_name.zone.local_zone.upstream_zone.thrift.upstream_resp_success``.
* thrift_proxy: add upstream metrics to show decoding errors and whether exception is from local or remote, e.g. ``cluster.cluster_name.thrift.upstream_resp_exception_remote``.
* thrift_proxy: add host level success/error metrics where success is a reply of type success and error is any other response to a call.
* thrift_proxy: support subset lb when using request or route metadata.
* udp: add support for multiple listener filters.
* upstream: added the ability to :ref:`configure max connection duration <envoy_v3_api_field_config.core.v3.HttpProtocolOptions.max_connection_duration>` for upstream clusters.
* vcl_socket_interface: added VCL socket interface extension for fd.io VPP integration to :ref:`contrib images <install_contrib>`. This can be enabled via :ref:`VCL <envoy_v3_api_msg_extensions.vcl.v3alpha.VclSocketInterface>` configuration.
* xds: re-introduced unified delta and sotw xDS multiplexers that share most of the implementation. Added a new runtime config ``envoy.reloadable_features.unified_mux`` (disabled by default) that when enabled, switches xDS to use unified multiplexers.

Deprecated
----------
* bootstrap: :ref:`dns_resolution_config <envoy_v3_api_field_config.bootstrap.v3.Bootstrap.dns_resolution_config>` is deprecated in favor of :ref:`typed_dns_resolver_config <envoy_v3_api_field_config.bootstrap.v3.Bootstrap.typed_dns_resolver_config>`.
* cluster: :ref:`dns_resolution_config <envoy_v3_api_field_config.cluster.v3.Cluster.dns_resolution_config>` is deprecated in favor of :ref:`typed_dns_resolver_config <envoy_v3_api_field_config.cluster.v3.Cluster.typed_dns_resolver_config>`.
* dns_cache: :ref:`dns_resolution_config <envoy_v3_api_field_extensions.common.dynamic_forward_proxy.v3.DnsCacheConfig.dns_resolution_config>` is deprecated in favor of :ref:`typed_dns_resolver_config <envoy_v3_api_field_extensions.common.dynamic_forward_proxy.v3.DnsCacheConfig.typed_dns_resolver_config>`.
