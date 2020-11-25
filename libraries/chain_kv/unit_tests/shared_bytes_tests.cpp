#include <b1/session/shared_bytes.hpp>
#include <boost/test/unit_test.hpp>

using namespace eosio::session;

BOOST_AUTO_TEST_SUITE(shared_bytes_tests)

BOOST_AUTO_TEST_CASE(make_shared_bytes_test) {
   static constexpr auto* char_value  = "hello world";
   static const auto      char_length = strlen(char_value) - 1;
   static constexpr auto  int_value   = int64_t{ 100000000 };

   auto b1     = shared_bytes(char_value, char_length);
   auto buffer = std::vector<shared_bytes::underlying_type_t>{ std::begin(b1), std::end(b1) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b1.size() == char_length);

   auto b2 = shared_bytes(reinterpret_cast<const int8_t*>(char_value), char_length);
   buffer  = std::vector<shared_bytes::underlying_type_t>{ std::begin(b2), std::end(b2) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b2.size() == char_length);

   auto b3 = shared_bytes(&int_value, 1);
   buffer  = std::vector<shared_bytes::underlying_type_t>{ std::begin(b3), std::end(b3) };
   BOOST_REQUIRE(*(reinterpret_cast<const decltype(int_value)*>(buffer.data())) == int_value);
   BOOST_REQUIRE(b3.size() == sizeof(decltype(int_value)));

   auto b4 = shared_bytes(char_value, char_length);
   buffer  = std::vector<shared_bytes::underlying_type_t>{ std::begin(b4), std::end(b4) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b4.size() == char_length);

   auto invalid = shared_bytes(static_cast<int8_t*>(nullptr), 0);
   BOOST_REQUIRE(std::begin(invalid) == std::end(invalid));
   BOOST_REQUIRE(invalid.size() == 0);

   BOOST_REQUIRE(b1 == b2);
   BOOST_REQUIRE(b1 == b4);
   BOOST_REQUIRE(invalid == shared_bytes{});
   BOOST_REQUIRE(b1 != b3);

   auto b5 = b1;
   buffer  = std::vector<shared_bytes::underlying_type_t>{ std::begin(b5), std::end(b5) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b5.size() == char_length);
   BOOST_REQUIRE(b1 == b5);

   auto b6{ b1 };
   buffer = std::vector<shared_bytes::underlying_type_t>{ std::begin(b6), std::end(b6) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b6.size() == char_length);
   BOOST_REQUIRE(b1 == b6);

   auto b7 = std::move(b1);
   buffer  = std::vector<shared_bytes::underlying_type_t>{ std::begin(b7), std::end(b7) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b7.size() == char_length);

   auto b8{ std::move(b2) };
   buffer = std::vector<shared_bytes::underlying_type_t>{ std::begin(b8), std::end(b8) };
   BOOST_REQUIRE(memcmp(buffer.data(), reinterpret_cast<const int8_t*>(char_value), char_length) == 0);
   BOOST_REQUIRE(b8.size() == char_length);
}

BOOST_AUTO_TEST_CASE(iterator_test) {
   auto s      = std::string{ "Hello world foobar" };
   auto bytes  = eosio::session::shared_bytes{ s.data(), s.size() };
   auto result = std::string{ std::begin(bytes), std::end(bytes) };
   BOOST_REQUIRE(s == result);
}

BOOST_AUTO_TEST_CASE(std_copy) {
   auto bytes  = eosio::session::shared_bytes{ 18 };
   auto parts  = std::vector<std::string>{ "Hello ", "world ", "foobar" };
   auto offset = size_t{ 0 };
   for (const auto& part : parts) {
      std::copy(std::begin(part), std::end(part), std::begin(bytes) + offset);
      offset += part.size();
   }
   auto expected_result = std::string{ "Hello world foobar" };
   auto result          = std::string{ std::begin(bytes), std::end(bytes) };
   BOOST_REQUIRE(expected_result == result);

   result = "";
   for (const auto& ch : bytes) { result.push_back(ch); }
   BOOST_REQUIRE(expected_result == result);

   auto extracted_parts = std::vector<std::string>{};
   offset               = size_t{ 0 };
   for (const auto& part : parts) {
      extracted_parts.emplace_back(part.size(), '\0');
      std::copy(std::begin(bytes) + offset, std::begin(bytes) + offset + part.size(),
                std::begin(extracted_parts.back()));
      offset += part.size();
   }
   BOOST_REQUIRE(parts == extracted_parts);
}

BOOST_AUTO_TEST_CASE(cow) {
  auto original_text = std::string{"Hello World"};
  const auto original = eosio::session::shared_bytes{original_text.data(), original_text.size()};

  const auto copy1 = original;
  const auto copy1_data = copy1.data();
  BOOST_REQUIRE(copy1_data == original.data());

  const auto copy1_it = std::begin(copy1);
  BOOST_REQUIRE(*copy1_it == 'H');
  BOOST_REQUIRE(copy1_it[0] == 'H'); 
  BOOST_REQUIRE(copy1.data() == original.data());

  auto copy2 = original;
  auto copy2_data = copy2.data();
  BOOST_REQUIRE(copy2_data != original.data());

  auto copy3 = original;
  const auto copy3_it = std::begin(copy3);
  BOOST_REQUIRE(*copy3_it == 'H');
  BOOST_REQUIRE(copy3.data() != original.data());

  auto copy4 = original;
  const auto copy4_it = std::begin(copy4);
  BOOST_REQUIRE(copy4_it[0] == 'H');
  BOOST_REQUIRE(copy4.data() != original.data());
}

BOOST_AUTO_TEST_SUITE_END();
