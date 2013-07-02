/// @file tags.hpp
/// @author Michael Koch
/// @copyright CC BY 3.0

#ifndef __TAGSOUP_TAGS_HPP__
#define __TAGSOUP_TAGS_HPP__

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <tagsoup/token.hpp>

namespace ts
{

	/// @class open_tag
	/// @brief opening tag
	class open_tag
	{
		private:
			std::string id;
			std::vector<std::pair<std::string, std::string>> attributes;
		public:
			using const_attribute_iterator = decltype(attributes.cbegin());

			open_tag(const std::string id, const std::vector<std::pair<std::string, std::string>> attributes) : id(id), attributes(attributes) {}
			const std::string& get_id() const {return id;}
			const_attribute_iterator cbegin_attributes() const {return attributes.cbegin();}
			const_attribute_iterator cend_attributes() const {return attributes.cend();}
	};

	class closing_tag
	{
		private:
			std::string id;
		public:
			closing_tag(const std::string id) : id(id) {}
			const std::string& get_id() const {return id;}
	};

	class empty_tag
	{
		private:
			std::string id;
			std::vector<std::pair<std::string, std::string>> attributes;
		public:
			using const_attribute_iterator = decltype(attributes.cbegin());

			empty_tag(const std::string id, const std::vector<std::pair<std::string, std::string>> attributes) : id(id), attributes(attributes) {}
			const std::string& get_id() const {return id;}
			const_attribute_iterator cbegin_attributes() const {return attributes.cbegin();}
			const_attribute_iterator cend_attributes() const {return attributes.cend();}
	};

	class comment
	{
		private:
			std::string content;
		public:
			comment(const std::string content) : content(content) {}
			const std::string& get_content() const {return content;}
	};

	class text
	{
		private:
			std::string content;
		public:
			text(const std::string content) : content(content) {}
			const std::string& get_content() const {return content;}
	};

	class pi
	{
		private:
			std::string id;
			std::string code;
		public:
			pi(const std::string id, const std::string code) : id(id), code(code) {}
			const std::string& get_id() const {return id;}
			const std::string& get_code() const {return code;}
	};

	class cdata
	{
		private:
			std::string code;
		public:
			cdata(const std::string code) : code(code) {}
			const std::string& get_code() const {return code;}
	};

	class dtd
	{
		private:
			std::string id;
		public:
			dtd(const std::string id) : id(id) {}
			const std::string& get_id() const {return id;}
	};

	class unknown_tag
	{
		private:
			std::string description;
		public:
			unknown_tag(const std::string description) : description(description) {}
			const std::string& get_description() const {return description;}
	};


	using tag_token = token<open_tag, closing_tag, empty_tag, comment, text, pi, cdata, dtd, unknown_tag>;
	using tag_token_signature = tag_token::signature;

	tag_token make_open_tag_token(std::string id, std::vector<std::pair<std::string, std::string>> attributes)
	{return make_token(open_tag(std::move(id), std::move(attributes)), tag_token_signature());}

	tag_token make_closing_tag_token(std::string id)
	{return make_token(closing_tag(std::move(id)), tag_token_signature());}

	tag_token make_empty_tag_token(std::string id, std::vector<std::pair<std::string, std::string>> attributes)
	{return make_token(empty_tag(std::move(id), std::move(attributes)), tag_token_signature());}

	tag_token make_comment_token(std::string content)
	{return make_token(comment(std::move(content)), tag_token_signature());}

	tag_token make_text_token(std::string content)
	{return make_token(text(std::move(content)), tag_token_signature());}

	tag_token make_pi_token(std::string id, std::string code)
	{return make_token(pi(std::move(id), std::move(code)), tag_token_signature());}

	tag_token make_cdata_token(std::string code)
	{return make_token(cdata(std::move(code)), tag_token_signature());}

	tag_token make_dtd_token(std::string id)
	{return make_token(dtd(std::move(id)), tag_token_signature());}

	tag_token make_unknown_tag_token(std::string description)
	{return make_token(unknown_tag(std::move(description)), tag_token_signature());}
}

#endif

