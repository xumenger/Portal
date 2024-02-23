// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Agent.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Agent_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Agent_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3013000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3013000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Agent_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Agent_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Agent_2eproto;
namespace com {
namespace xum {
namespace proto {
namespace agent {
class AgentRegister;
class AgentRegisterDefaultTypeInternal;
extern AgentRegisterDefaultTypeInternal _AgentRegister_default_instance_;
}  // namespace agent
}  // namespace proto
}  // namespace xum
}  // namespace com
PROTOBUF_NAMESPACE_OPEN
template<> ::com::xum::proto::agent::AgentRegister* Arena::CreateMaybeMessage<::com::xum::proto::agent::AgentRegister>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace com {
namespace xum {
namespace proto {
namespace agent {

// ===================================================================

class AgentRegister PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:com.xum.proto.agent.AgentRegister) */ {
 public:
  inline AgentRegister() : AgentRegister(nullptr) {}
  virtual ~AgentRegister();

  AgentRegister(const AgentRegister& from);
  AgentRegister(AgentRegister&& from) noexcept
    : AgentRegister() {
    *this = ::std::move(from);
  }

  inline AgentRegister& operator=(const AgentRegister& from) {
    CopyFrom(from);
    return *this;
  }
  inline AgentRegister& operator=(AgentRegister&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const AgentRegister& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const AgentRegister* internal_default_instance() {
    return reinterpret_cast<const AgentRegister*>(
               &_AgentRegister_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(AgentRegister& a, AgentRegister& b) {
    a.Swap(&b);
  }
  inline void Swap(AgentRegister* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(AgentRegister* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AgentRegister* New() const final {
    return CreateMaybeMessage<AgentRegister>(nullptr);
  }

  AgentRegister* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AgentRegister>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AgentRegister& from);
  void MergeFrom(const AgentRegister& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AgentRegister* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "com.xum.proto.agent.AgentRegister";
  }
  protected:
  explicit AgentRegister(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_Agent_2eproto);
    return ::descriptor_table_Agent_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAgentNameFieldNumber = 1,
    kHostIpFieldNumber = 2,
  };
  // string agent_name = 1;
  void clear_agent_name();
  const std::string& agent_name() const;
  void set_agent_name(const std::string& value);
  void set_agent_name(std::string&& value);
  void set_agent_name(const char* value);
  void set_agent_name(const char* value, size_t size);
  std::string* mutable_agent_name();
  std::string* release_agent_name();
  void set_allocated_agent_name(std::string* agent_name);
  private:
  const std::string& _internal_agent_name() const;
  void _internal_set_agent_name(const std::string& value);
  std::string* _internal_mutable_agent_name();
  public:

  // string host_ip = 2;
  void clear_host_ip();
  const std::string& host_ip() const;
  void set_host_ip(const std::string& value);
  void set_host_ip(std::string&& value);
  void set_host_ip(const char* value);
  void set_host_ip(const char* value, size_t size);
  std::string* mutable_host_ip();
  std::string* release_host_ip();
  void set_allocated_host_ip(std::string* host_ip);
  private:
  const std::string& _internal_host_ip() const;
  void _internal_set_host_ip(const std::string& value);
  std::string* _internal_mutable_host_ip();
  public:

  // @@protoc_insertion_point(class_scope:com.xum.proto.agent.AgentRegister)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr agent_name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr host_ip_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_Agent_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// AgentRegister

// string agent_name = 1;
inline void AgentRegister::clear_agent_name() {
  agent_name_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& AgentRegister::agent_name() const {
  // @@protoc_insertion_point(field_get:com.xum.proto.agent.AgentRegister.agent_name)
  return _internal_agent_name();
}
inline void AgentRegister::set_agent_name(const std::string& value) {
  _internal_set_agent_name(value);
  // @@protoc_insertion_point(field_set:com.xum.proto.agent.AgentRegister.agent_name)
}
inline std::string* AgentRegister::mutable_agent_name() {
  // @@protoc_insertion_point(field_mutable:com.xum.proto.agent.AgentRegister.agent_name)
  return _internal_mutable_agent_name();
}
inline const std::string& AgentRegister::_internal_agent_name() const {
  return agent_name_.Get();
}
inline void AgentRegister::_internal_set_agent_name(const std::string& value) {
  
  agent_name_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void AgentRegister::set_agent_name(std::string&& value) {
  
  agent_name_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:com.xum.proto.agent.AgentRegister.agent_name)
}
inline void AgentRegister::set_agent_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  agent_name_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:com.xum.proto.agent.AgentRegister.agent_name)
}
inline void AgentRegister::set_agent_name(const char* value,
    size_t size) {
  
  agent_name_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:com.xum.proto.agent.AgentRegister.agent_name)
}
inline std::string* AgentRegister::_internal_mutable_agent_name() {
  
  return agent_name_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* AgentRegister::release_agent_name() {
  // @@protoc_insertion_point(field_release:com.xum.proto.agent.AgentRegister.agent_name)
  return agent_name_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void AgentRegister::set_allocated_agent_name(std::string* agent_name) {
  if (agent_name != nullptr) {
    
  } else {
    
  }
  agent_name_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), agent_name,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:com.xum.proto.agent.AgentRegister.agent_name)
}

// string host_ip = 2;
inline void AgentRegister::clear_host_ip() {
  host_ip_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& AgentRegister::host_ip() const {
  // @@protoc_insertion_point(field_get:com.xum.proto.agent.AgentRegister.host_ip)
  return _internal_host_ip();
}
inline void AgentRegister::set_host_ip(const std::string& value) {
  _internal_set_host_ip(value);
  // @@protoc_insertion_point(field_set:com.xum.proto.agent.AgentRegister.host_ip)
}
inline std::string* AgentRegister::mutable_host_ip() {
  // @@protoc_insertion_point(field_mutable:com.xum.proto.agent.AgentRegister.host_ip)
  return _internal_mutable_host_ip();
}
inline const std::string& AgentRegister::_internal_host_ip() const {
  return host_ip_.Get();
}
inline void AgentRegister::_internal_set_host_ip(const std::string& value) {
  
  host_ip_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void AgentRegister::set_host_ip(std::string&& value) {
  
  host_ip_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:com.xum.proto.agent.AgentRegister.host_ip)
}
inline void AgentRegister::set_host_ip(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  host_ip_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:com.xum.proto.agent.AgentRegister.host_ip)
}
inline void AgentRegister::set_host_ip(const char* value,
    size_t size) {
  
  host_ip_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:com.xum.proto.agent.AgentRegister.host_ip)
}
inline std::string* AgentRegister::_internal_mutable_host_ip() {
  
  return host_ip_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* AgentRegister::release_host_ip() {
  // @@protoc_insertion_point(field_release:com.xum.proto.agent.AgentRegister.host_ip)
  return host_ip_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void AgentRegister::set_allocated_host_ip(std::string* host_ip) {
  if (host_ip != nullptr) {
    
  } else {
    
  }
  host_ip_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), host_ip,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:com.xum.proto.agent.AgentRegister.host_ip)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace agent
}  // namespace proto
}  // namespace xum
}  // namespace com

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Agent_2eproto