#pragma once

#include "source/common/quic/envoy_quic_client_connection.h"
#include "source/common/quic/envoy_quic_client_stream.h"
#include "source/common/quic/envoy_quic_crypto_stream_factory.h"
#include "source/common/quic/quic_filter_manager_connection_impl.h"
#include "source/common/quic/quic_stat_names.h"

#include "quiche/quic/core/http/quic_spdy_client_session.h"

namespace Envoy {
namespace Quic {

// Act as a Network::ClientConnection to ClientCodec.
// TODO(danzh) This class doesn't need to inherit Network::FilterManager
// interface but need all other Network::Connection implementation in
// QuicFilterManagerConnectionImpl. Refactor QuicFilterManagerConnectionImpl to
// move FilterManager interface to EnvoyQuicServerSession.
class EnvoyQuicClientSession : public QuicFilterManagerConnectionImpl,
                               public quic::QuicSpdyClientSession,
                               public Network::ClientConnection,
                               public PacketsToReadDelegate {
public:
  EnvoyQuicClientSession(const quic::QuicConfig& config,
                         const quic::ParsedQuicVersionVector& supported_versions,
                         std::unique_ptr<EnvoyQuicClientConnection> connection,
                         const quic::QuicServerId& server_id,
                         std::shared_ptr<quic::QuicCryptoClientConfig> crypto_config,
                         quic::QuicClientPushPromiseIndex* push_promise_index,
                         Event::Dispatcher& dispatcher, uint32_t send_buffer_limit,
                         EnvoyQuicCryptoClientStreamFactoryInterface& crypto_stream_factory,
                         QuicStatNames& quic_stat_names, Stats::Scope& scope);

  ~EnvoyQuicClientSession() override;

  // Called by QuicHttpClientConnectionImpl before creating data streams.
  void setHttpConnectionCallbacks(Http::ConnectionCallbacks& callbacks) {
    http_connection_callbacks_ = &callbacks;
  }

  // Network::Connection
  absl::string_view requestedServerName() const override;
  void dumpState(std::ostream&, int) const override {
    // TODO(kbaichoo): Implement dumpState for H3.
  }

  // Network::ClientConnection
  // Set up socket and start handshake.
  void connect() override;

  // quic::QuicSession
  void OnConnectionClosed(const quic::QuicConnectionCloseFrame& frame,
                          quic::ConnectionCloseSource source) override;
  void Initialize() override;
  void OnCanWrite() override;
  void OnHttp3GoAway(uint64_t stream_id) override;
  void OnTlsHandshakeComplete() override;
  void MaybeSendRstStreamFrame(quic::QuicStreamId id, quic::QuicResetStreamError error,
                               quic::QuicStreamOffset bytes_written) override;
  void OnRstStream(const quic::QuicRstStreamFrame& frame) override;
  // quic::QuicSpdyClientSessionBase
  void SetDefaultEncryptionLevel(quic::EncryptionLevel level) override;
  // quic::ProofHandler
  void OnProofVerifyDetailsAvailable(const quic::ProofVerifyDetails& verify_details) override;

  // PacketsToReadDelegate
  size_t numPacketsExpectedPerEventLoop() override {
    // Do one round of reading per active stream, or to see if there's a new
    // active stream.
    return std::max<size_t>(1, GetNumActiveStreams()) * Network::NUM_DATAGRAMS_PER_RECEIVE;
  }

  // QuicFilterManagerConnectionImpl
  void setHttp3Options(const envoy::config::core::v3::Http3ProtocolOptions& http3_options) override;

  using quic::QuicSpdyClientSession::PerformActionOnActiveStreams;

protected:
  // quic::QuicSpdyClientSession
  std::unique_ptr<quic::QuicSpdyClientStream> CreateClientStream() override;
  // quic::QuicSpdySession
  quic::QuicSpdyStream* CreateIncomingStream(quic::QuicStreamId id) override;
  quic::QuicSpdyStream* CreateIncomingStream(quic::PendingStream* pending) override;
  std::unique_ptr<quic::QuicCryptoClientStreamBase> CreateQuicCryptoStream() override;
  bool ShouldCreateOutgoingBidirectionalStream() override {
    ASSERT(quic::QuicSpdyClientSession::ShouldCreateOutgoingBidirectionalStream());
    // Prefer creating an "invalid" stream outside of current stream bounds to
    // crashing when dereferencing a nullptr in QuicHttpClientConnectionImpl::newStream
    return true;
  }
  // QuicFilterManagerConnectionImpl
  bool hasDataToWrite() override;
  // Used by base class to access quic connection after initialization.
  const quic::QuicConnection* quicConnection() const override;
  quic::QuicConnection* quicConnection() override;

private:
  // These callbacks are owned by network filters and quic session should outlive
  // them.
  Http::ConnectionCallbacks* http_connection_callbacks_{nullptr};
  // TODO(danzh) deprecate this field once server_id() is made const.
  const std::string host_name_;
  std::shared_ptr<quic::QuicCryptoClientConfig> crypto_config_;
  EnvoyQuicCryptoClientStreamFactoryInterface& crypto_stream_factory_;
  QuicStatNames& quic_stat_names_;
  Stats::Scope& scope_;
};

} // namespace Quic
} // namespace Envoy
