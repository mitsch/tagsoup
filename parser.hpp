/// @file parser.hpp
/// @author Michael Koch
/// @copyright CC BY 3.0

#ifndef __TAGSOUP_PARSER_HPP__
#define __TAGSOUP_PARSER_HPP__

#include <cctype>
#include <cassert>
#include <tuple>
#include <string>
#include <algorithm>
#include <tagsoup/tags.hpp>

namespace ts
{

	

	/// @class parser
	/// @brief parses tagged documents (it only consists of a tokenizer)
	class parser
	{

		private:

			/// all different states whilst parsing
			enum class state_type
			{
				/// the very beginning of parsing
				initial = 0,
				/// parsed following regular expression: '<'
				open_abracket,
				/// parsed following regular expression: '<!'
				open_abracket__exclamation_mark,
				/// parsed following regular expression: '<!D'
				open_abracket__exclamation_mark__big_d,
				/// parsed following regular expression: '<!DO'
				open_abracket__exclamation_mark__big_do,
				/// parsed following regular expression: '<!DOC'
				open_abracket__exclamation_mark__big_doc,
				/// parsed following regular expression: '<!DOCT'
				open_abracket__exclamation_mark__big_doct,
				/// parsed following regular expression: '<!DOCTY'
				open_abracket__exclamation_mark__big_docty,
				/// parsed following regular expression: '<!DOCTYP'
				open_abracket__exclamation_mark__big_doctyp,
				/// parsed following regular expression: '<!DOCTYPE'
				open_abracket__exclamation_mark__big_doctype,
				/// parsed following regular expression: '<!DOCTYPE' Space+
				open_abracket__exclamation_mark__big_doctype__space,
				/// parsed following regular expression: '<!DOCTYPE' Space+ Name
				open_abracket__exclamation_mark__big_doctype__space__name,
				/// parsed following regular expression: '<!DOCTYPE' Space+ Name Space+
				open_abracket__exclamation_mark__big_doctype__space__name__space,
				/// parsed following regular expression: '<!-'
				open_abracket__exclamation_mark__bar,
				/// parsed following regular expression: '<!--' (Char\{'-->'})*
				open_abracket__exclamation_mark__bar__bar,
				/// parsed following regular expression: '<!--' (Char\{'-->'})* '-'
				open_abracket__exclamation_mark__bar__bar__bar,
				/// parsed following regular expression: '<!--' (Char\{'-->'})* '--'
				open_abracket__exclamation_mark__bar__bar__bar__bar,
				/// parsed following regular expression: '<!['
				open_abracket__exclamation_mark__sbracket,
				/// parsed following regular expression: '<![C'
				open_abracket__exclamation_mark__sbracket__big_c,
				/// parsed following regular expression: '<![CD'
				open_abracket__exclamation_mark__sbracket__big_c__big_d,
				/// parsed following regular expression: '<![CDA'
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a,
				/// parsed following regular expression: '<![CDAT'
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t,
				/// parsed following regular expression: '<![CDATA'
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a,
				/// parsed following regular expression: '<![CDATA[' (Char\{']]>'})*
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket,
				/// parsed following regular expression: '<![CDATA[' (Char\{']]>'})* ']'
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket,
				/// parsed following regular expression: '<![CDATA[' (Char\{']]>'})* ']]'
				open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket__closed_sbracket,
				///	parsed following regular expression: '<?'
				open_abracket__question_mark,
				///	parsed following regular expression: '<?' Name
				open_abracket__question_mark__name,
				///	parsed following regular expression: '<?' Name Space+
				open_abracket__question_mark__name__space,
				///	parsed following regular expression: '<?' Name Space+ (Char\{'?>'})*
				open_abracket__question_mark__name__code,
				///	parsed following regular expression: '<?' Name Space+ (Char\{'?>'})* '?'
				open_abracket__question_mark__name__code__question_mark,
				/// parsed following regular expression: '</'
				open_abracket__slash,
				/// parsed following regular expression: '</' Name
				open_abracket__slash__name,
				/// parsed following regular expression: '</' Name Space+
				open_abracket__slash__name__space,
				/// parsed following regular expression: '<' Name
				open_abracket__name,	
				/// parsed following regular expression: '<' Name '/'
				open_abracket__name__slash,
				/// parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+
				open_abracket__name__space,
				/// parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName
				open_abracket__name__attrname,
				/// parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space+
				open_abracket__name__attrname__space,
				/// parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' Space*
				open_abracket__name__attrequal,
				///	parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' Space* '"'
				open_abracket__name__dq,
				///	parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' Space* '''
				open_abracket__name__sq,
				///	parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' AttrChar+
				open_abracket__name__uq,
				/// parsed following regular expression: '<' Name (Space+ AttrName Space* '=' Space* AttrValue)+
				open_abracket__name__attrend,
				/// parsed following regular expression: (Char\{'<'})+
				characters,
				/// parsed following regular expression: '<' Name Space* (Space+ AttrName Space* '=' Space* AttrValue)* '>'
				open_tag,
				/// parsed following regular expression: '</' Name '>'
				closed_tag,
				/// parsed following regular expression: '<' Name Space* (Space+ AttrName Space* '=' Space* AttrValue)* '/>'
				empty_tag,
				/// parsed following regular expression: (Char\{'<'})+
				text,
				/// parsed following regular expression: '<?' Name Space+ (Char\{'?>'})* '?>'
				process_instruction,
				/// parsed following regular expression: '<![CDATA[' (Char\{']]>'})* ']]>'
				cdata,
				comment,
				dtd
			};

			enum class script_state
			{
				reading_script,
				open_abracket,
				open_abracket__slash,
				open_abracket__slash__name,
				open_abracket__slash__name__space,
				closing_tag
			};

			bool skipping_text;
			bool skipping_cdata;
			bool skipping_comment;
			bool skipping_pi;
			bool allowing_weak_comment_coding;
			bool allowing_weak_pi_coding;
			bool allowing_weak_double_quote_coding;
			bool allowing_weak_single_quote_coding;
			bool allowing_unquoted_attribute_value;
			bool allowing_concated_attribute;

			/// @brief test whether state is accepting or not
			/// @retval true state is accepting
			/// @retval false state is not accepting
			/// @param state state to test
			inline bool is_accepting_state(const state_type state) const
			{
				return 	state == state_type::open_tag ||
						state == state_type::closed_tag ||
						state == state_type::empty_tag ||
						state == state_type::text ||
						state == state_type::process_instruction ||
						state == state_type::cdata ||
						state == state_type::comment ||
						state == state_type::dtd;
			}

			inline bool is_open_abracket(const char c) const {return c == '<';}
			inline bool is_closed_abracket(const char c) const {return c == '>';}
			inline bool is_exclamation_mark(const char c) const {return c == '!';}
			inline bool is_question_mark(const char c) const {return c == '?';}
			inline bool is_slash(const char c) const {return c == '/';}
			inline bool is_starting_name(const char c) const {return std::isalpha(c);}
			inline bool is_name(const char c) const {return std::isalnum(c) || c == '.' || c == '-';}
			inline bool is_char(const char c) const {return true;}
			inline bool is_bar(const char c) const {return c == '-';}
			inline bool is_open_sbracket(const char c) const {return c == '[';}
			inline bool is_closed_sbracket(const char c) const {return c == ']';}
			inline bool is_big_c(const char c) const {return c == 'C';}
			inline bool is_big_d(const char c) const {return c == 'D';}
			inline bool is_big_a(const char c) const {return c == 'A';}
			inline bool is_big_t(const char c) const {return c == 'T';}
			inline bool is_space(const char c) const {return std::isspace(c);}
			inline bool is_assignment(const char c) const {return c== '=';}
			inline bool is_double_quote(const char c) const {return c == '\"';}
			inline bool is_single_quote(const char c) const {return c == '\'';}
			inline bool is_unquoted_attribute_value(const char c) const {return !std::isspace(c) && c != '\"' && c != '\'' && c != '=' && c != '<' && c != '>' && c != 0x60;}

			inline char get_closed_sbracket() const {return ']';}
			inline char get_bar() const {return '-';}
			inline char get_question_mark() const {return '?';}

			/// @brief gives human readable error description of error depending on the state where it is happening
			/// @param state where the error has happened
			/// @return text containing the description
			static std::string formulate_error(const state_type state)
			{
				if (state == state_type::open_abracket)
					return std::string("expecting \'!\' or \'?\' or \'/\' or some id name!");
				else if (state == state_type::open_abracket__exclamation_mark)
					return std::string("expecting \'-\' or \'[\'!");
				else if (state == state_type::open_abracket__exclamation_mark__bar)
					return std::string("expecting \'-\'!");
				else if (state == state_type::open_abracket__exclamation_mark__bar__bar)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__exclamation_mark__bar__bar__bar)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__exclamation_mark__bar__bar__bar__bar)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket)
					return std::string("expecting \'C\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c)
					return std::string("expecting \'D\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d)
					return std::string("expecting \'A\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a)
					return std::string("expecting \'T\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t)
					return std::string("expecting \'A\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a)
					return std::string("expecting \'[\'!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket__closed_sbracket)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__question_mark)
					return std::string("expecting some id name!");
				else if (state == state_type::open_abracket__question_mark__name)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__question_mark__name__space)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__question_mark__name__code)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__question_mark__name__code__question_mark)
					return std::string("expecting some char!");
				else if (state == state_type::open_abracket__slash)
					return std::string("expecting some id name!");
				else if (state == state_type::open_abracket__slash__name)
					return std::string("expecting some space or name or closing angle bracket!");
				else if (state == state_type::open_abracket__slash__name__space)
					return std::string("expecting some space or closing angle bracket!");
				else if (state == state_type::open_abracket__name)
					return std::string("expecting some name or slash or some space or a closing angle bracket!");
				else if (state == state_type::open_abracket__name__slash)
					return std::string("expecting a closing angle bracket!");
				else if (state == state_type::open_abracket__name__space)
					return std::string("expecting some space or a starting name character or a slash or a closing angle bracket!");
				else if (state == state_type::open_abracket__name__attrname)
					return std::string("expecting some name character or space or \'=\' or \'/\' or \'>\'!");
				else if (state == state_type::open_abracket__name__attrname__space)
					return std::string("expecting some space or \'=\' or \'/\' or \'>\'!");
				else if (state == state_type::open_abracket__name__attrequal)
					return std::string("expecting some space or \'\'\' or \'\"\' or some attribute value character!");
				else if (state == state_type::open_abracket__name__dq)
					return std::string("expecting some attribute value character or \'\"\'!");
				else if (state == state_type::open_abracket__name__sq)
					return std::string("expecting some attribute value character or \'\'\'!");
				else if (state == state_type::open_abracket__name__uq)
					return std::string("expecting some attribute value character or some space or \'/\' or \'>\'!");
				else if (state == state_type::open_abracket__name__attrend)
					return std::string("expecting some space or \'/\' or \'>\'!");
				else if (state == state_type::characters)
					return std::string("expecting some chars or \'<\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_d)
					return std::string("expecting \'O\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_do)
					return std::string("expecting \'C\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doc)
					return std::string("expecting \'T\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doct)
					return std::string("expecting \'Y\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_docty)
					return std::string("expecting \'P\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doctyp)
					return std::string("expecting \'E\'!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doctype)
					return std::string("expecting space!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doctype__space)
					return std::string("expecting space or starting name character!");
				else if (state == state_type::open_abracket__exclamation_mark__big_doctype__space__name)
					return std::string("expecting name character or \'>\' or space or \'[\'!");
				else
					assert(false);
			}

		public:

			parser(const bool skipping_text = false, const bool skipping_cdata = false, const bool skipping_comment = false,
					const bool skipping_pi = false, const bool allowing_weak_comment_coding = true, const bool allowing_weak_pi_coding = true,
					const bool allowing_weak_double_quote_coding = true, const bool allowing_weak_single_quote_coding = true,
					const bool allowing_unquoted_attribute_value = true, const bool allowing_concated_attribute = true) :
				skipping_text(skipping_text), skipping_cdata(skipping_cdata), skipping_comment(skipping_comment), skipping_pi(skipping_pi),
				allowing_weak_comment_coding(allowing_weak_comment_coding), allowing_weak_pi_coding(allowing_weak_pi_coding),
				allowing_weak_double_quote_coding(allowing_weak_double_quote_coding),
				allowing_weak_single_quote_coding(allowing_weak_single_quote_coding),
				allowing_unquoted_attribute_value(allowing_unquoted_attribute_value),
				allowing_concated_attribute(allowing_concated_attribute)
			{}
			parser(const parser &) = default;
			parser(parser &&) = default;
			~parser() = default;

			parser& operator = (const parser &) = default;
			parser& operator = (parser &&) = default;

			inline bool skip_text() const {return skipping_text;}
			inline bool skip_cdata() const {return skipping_cdata;}
			inline bool skip_comment() const {return skipping_comment;}
			inline bool skip_pi() const {return skipping_pi;}

			inline bool allow_weak_comment_coding() const {return allowing_weak_comment_coding;}
			inline bool allow_weak_pi_coding() const {return allowing_weak_pi_coding;}
			inline bool allow_weak_double_quote_coding() const {return allowing_weak_double_quote_coding;}
			inline bool allow_weak_single_quote_coding() const {return allowing_weak_single_quote_coding;}
			inline bool allow_unquoted_attribute_value() const {return allowing_unquoted_attribute_value;}
			inline bool allow_concated_attribute() const {return allowing_concated_attribute;}

			inline void skip_text(const bool skip) {skipping_text = skip;}
			inline void skip_cdata(const bool skip) {skipping_cdata = skip;}
			inline void skip_comment(const bool skip) {skipping_comment = skip;}
			inline void skip_pi(const bool skip) {skipping_pi = skip;}
			
			inline void allow_weak_comment_coding(const bool allow) {allowing_weak_comment_coding = allow;}
			inline void allow_weak_pi_coding(const bool allow) {allowing_weak_pi_coding = allow;}
			inline void allow_weak_double_quote_coding(const bool allow) {allowing_weak_double_quote_coding = allow;}
			inline void allow_weak_single_quote_coding(const bool allow) {allowing_weak_single_quote_coding = allow;}
			inline void allow_unquoted_attribute_value(const bool allow) {allowing_unquoted_attribute_value = allow;}
			inline void allow_concated_attribute(const bool allow) {allowing_concated_attribute = allow;}

			/// @brief parse incoming text for tag entities
			/// @tparam InputIterator type concept input iterator
			/// @return ...
			/// @param start first iterator position of text to parse
			/// @param end first iterator after last position of text to parse
			template <typename InputIterator>
			std::tuple<InputIterator, tag_token> parse(InputIterator start, InputIterator end, size_t & line, size_t & column) const
			{
				static_assert(std::is_convertible<decltype(*start), char>::value, "iterator must refer to values of type char!");

				state_type state = state_type::initial;
				std::string param1;
				std::string param2;
				std::string param3;
				std::vector<std::pair<std::string, std::string>> pairs1;

				bool error = false;
				auto iter = start;
				while (!is_accepting_state(state) && iter != end && !error)
				{
					auto c = *iter;
					switch (state)
					{
						// state so far is:
						// ''
						case state_type::initial:
							if (is_open_abracket(c)) state = state_type::open_abracket;
							else {if (!skipping_text) param1.push_back(c); state = state_type::characters;}
							break;

						// state so far is:
						// '<'
						case state_type::open_abracket:
							if (is_exclamation_mark(c)) state = state_type::open_abracket__exclamation_mark;
							else if (is_question_mark(c)) state = state_type::open_abracket__question_mark;
							else if (is_slash(c)) state = state_type::open_abracket__slash;
							else if (is_starting_name(c)) {param1.push_back(c); state = state_type::open_abracket__name;}
							else {error = true;}
							break;

						// state so far is:
						// '<!'
						case state_type::open_abracket__exclamation_mark:
							if (is_bar(c)) state = state_type::open_abracket__exclamation_mark__bar;
							else if (is_open_sbracket(c)) state = state_type::open_abracket__exclamation_mark__sbracket;
							else if (c == 'D' || c == 'd') state = state_type::open_abracket__exclamation_mark__big_d;
							else {error = true;}
							break;

						// state so far is:
						// '<!D'
						case state_type::open_abracket__exclamation_mark__big_d:
							if (c == 'O' || c == 'o') state = state_type::open_abracket__exclamation_mark__big_do;
							else error = true;
							break;

						// state so far is:
						// '<!DO'
						case state_type::open_abracket__exclamation_mark__big_do:
							if (c == 'C' || c == 'c') state = state_type::open_abracket__exclamation_mark__big_doc;
							else error = true;
							break;

						// state so far is:
						// '<!DOC'
						case state_type::open_abracket__exclamation_mark__big_doc:
							if (c == 'T' || c == 't') state = state_type::open_abracket__exclamation_mark__big_doct;
							else error = true;
							break;

						// state so far is:
						// '<!DOCT'
						case state_type::open_abracket__exclamation_mark__big_doct:
							if (c == 'Y' || c == 'y') state = state_type::open_abracket__exclamation_mark__big_docty;
							else error = true;
							break;

						// state so far is:
						// '<!DOCTY'
						case state_type::open_abracket__exclamation_mark__big_docty:
							if (c == 'P' || c == 'p') state = state_type::open_abracket__exclamation_mark__big_doctyp;
							else error = true;
							break;

						// state so far is:
						// '<!DOCTYP'
						case state_type::open_abracket__exclamation_mark__big_doctyp:
							if (c == 'E' || c == 'e') state = state_type::open_abracket__exclamation_mark__big_doctype;
							else error = true;
							break;

						// state so far is:
						// '<!DOCTYPE'
						case state_type::open_abracket__exclamation_mark__big_doctype:
							if (c == '>') {state = state_type::dtd;}
							else {}
							break;

						// state so far is:
						// '<!['
						case state_type::open_abracket__exclamation_mark__sbracket:
							if (c == 'C') state = state_type::open_abracket__exclamation_mark__sbracket__big_c;
							else {error = true;}
							break;

						// state so far is:
						// '<![C'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c:
							if (c == 'D') state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d;
							else {error = true;}
							break;

						// state so far is:
						// '<![CD'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d:
							if (c == 'A') state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a;
							else {error = true;}
							break;

						// state so far is:
						// '<![CDA'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a:
							if (c == 'T') state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t;
							else {error = true;}
							break;

						// state so far is:
						// '<![CDAT'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t:
							if (c == 'A') state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a;
							else {error = true;}
							break;
						
						// state so far is:
						// '<![CDATA'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a:
							if (is_open_sbracket(c))
							{state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket;}
							else {error = true;}
							break;
						
						// state so far is:
						// '<![CDATA[' (Char\{']]>'})*
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket:
							if (is_closed_sbracket(c))
								state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket;
							else if (is_char(c)) {if(!skipping_cdata) param1.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '<![CDATA[' (Char\{']]>'})* ']'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket:
							if (is_closed_sbracket(c))
								state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket__closed_sbracket;
							else if (is_char(c))
							{
								if (!skipping_cdata)
								{
									param1.push_back(get_closed_sbracket());
									param1.push_back(c);
								}
								state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket;
							}
							else {error = true;}
							break;
						
						// state so far is:
						// '<![CDATA[' (Char\{']]>'})* ']]'
						case state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket__closed_sbracket__closed_sbracket:
							if (is_closed_abracket(c)) state = state_type::cdata;
							else if (is_char(c))
							{
								if (!skipping_cdata)
								{
									param1.push_back(get_closed_sbracket());
									param1.push_back(get_closed_sbracket());
									param1.push_back(c);
								}
								state = state_type::open_abracket__exclamation_mark__sbracket__big_c__big_d__big_a__big_t__big_a__open_sbracket;
							}
							else {error = true;}
							break;

						// state so far is:
						// '<!-'
						case state_type::open_abracket__exclamation_mark__bar:
							if (is_bar(c)) {state = state_type::open_abracket__exclamation_mark__bar__bar;}
							else {error = true;}
							break;

						// state so far is:
						// '<!--' (Char\{'-->'})*
						case state_type::open_abracket__exclamation_mark__bar__bar:
							if (is_bar(c)) state = state_type::open_abracket__exclamation_mark__bar__bar__bar;
							else if (is_char(c) || allowing_weak_comment_coding) {if (!skipping_comment) param1.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '<!--' (Char\{'-->'})* '-'
						case state_type::open_abracket__exclamation_mark__bar__bar__bar:
							if (is_bar(c)) state = state_type::open_abracket__exclamation_mark__bar__bar__bar__bar;
							else if (is_char(c) || allowing_weak_comment_coding)
							{
								if (!skipping_comment)
								{
									param1.push_back(get_bar());
									param1.push_back(c);
									state = state_type::open_abracket__exclamation_mark__bar__bar;
								}
							}
							else {error = true;}
							break;

						// state so far is:
						// '<!--' (Char\{'-->'})* '--'
						case state_type::open_abracket__exclamation_mark__bar__bar__bar__bar:
							if (is_closed_abracket(c)) state = state_type::comment;
							else if (is_char(c) || allowing_weak_comment_coding)
							{
								if (!skipping_comment)
								{
									param1.push_back(get_bar());
									param1.push_back(c);
									state = state_type::open_abracket__exclamation_mark__bar__bar;
								}
							}
							else {error = true;}
							break;

						// state so far is:
						// '<?'
						case state_type::open_abracket__question_mark:
							if (is_starting_name(c)) {param1.push_back(c); state = state_type::open_abracket__question_mark__name;}
							else {error = true;}
							break;

						// state so far is:
						// '<?' Name
						case state_type::open_abracket__question_mark__name:
							if (is_name(c)) {param1.push_back(c);}
							else if (is_space(c)) {state = state_type::open_abracket__question_mark__name__space;}
							else if (is_question_mark(c)) {state = state_type::open_abracket__question_mark__name__code__question_mark;}
							else {error = true;}
							break;

						// state so far is:
						// '<?' Name Space
						case state_type::open_abracket__question_mark__name__space:
							if (is_space(c)) {}
							else if (is_question_mark(c)) {state = state_type::open_abracket__question_mark__name__code__question_mark;}
							else if (is_char(c) || allowing_weak_pi_coding)
							{
								if (!skipping_pi)
								{
									param2.push_back(c);
									state = state_type::open_abracket__question_mark__name__code;
								}
							}
							else {error = true;}
							break;

						// state so far is:
						// '<?' Name Space+ (Char\{'?>'})+
						case state_type::open_abracket__question_mark__name__code:
							if (is_question_mark(c)) {state = state_type::open_abracket__question_mark__name__code__question_mark;}
							else if (is_char(c) || allowing_weak_pi_coding) {if (!skipping_pi) param2.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '<?' Name (Space+ (Char\{'?>'})*)? '?'
						case state_type::open_abracket__question_mark__name__code__question_mark:
							if (is_closed_abracket(c)) {state = state_type::process_instruction;}
							else if (is_char(c) || allowing_weak_pi_coding)
							{
								if (!skipping_pi)
								{
									param2.push_back(get_question_mark());
									param2.push_back(c);
									state = state_type::open_abracket__question_mark__name__code;
								}
							}
							else {error = true;}
							break;

						// state so far is:
						// '</'
						case state_type::open_abracket__slash:
							if (is_starting_name(c)) {param1.push_back(c); state = state_type::open_abracket__slash__name;}
							else {error = true;}
							break;

						// state so far is:
						// '</' Name
						case state_type::open_abracket__slash__name:
							if (is_space(c)) {state = state_type::open_abracket__slash__name__space;}
							else if (is_closed_abracket(c)) {state = state_type::closed_tag;}
							else if (is_name(c)) {param1.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '</' Name Space+
						case state_type::open_abracket__slash__name__space:
							if (is_closed_abracket(c)) {state = state_type::closed_tag;}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name
						case state_type::open_abracket__name:	
							if (is_space(c)) {state = state_type::open_abracket__name__space;}
							else if (is_closed_abracket(c)) {state = state_type::open_tag;}
							else if (is_slash(c)) {state = state_type::open_abracket__name__slash;}
							else if (is_name(c)) {param1.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space* '/'
						case state_type::open_abracket__name__slash:
							if (is_closed_abracket(c)) {state = state_type::empty_tag;}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space+
						case state_type::open_abracket__name__space:
							if (is_space(c)) {}
							else if (is_closed_abracket(c)) {state = state_type::open_tag;}
							else if (is_slash(c)) {state = state_type::open_abracket__name__slash;}
							else if (is_starting_name(c)) {param2.push_back(c); state = state_type::open_abracket__name__attrname;}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space+ (AttrName Space* '=' Space* AttrValue)* AttrName
						case state_type::open_abracket__name__attrname:
							if (is_space(c)) {state = state_type::open_abracket__name__attrname__space;}
							else if (is_assignment(c)) {state = state_type::open_abracket__name__attrequal;}
							else if (is_name(c)) {param2.push_back(c);}
							else if (c == '>')
							{
								pairs1.push_back(std::make_pair(std::move(param1), std::string("")));
								state = state_type::closed_tag;
							}
							else if (is_slash(c))
							{
								pairs1.push_back(std::make_pair(std::move(param1), std::string("")));
								state = state_type::open_abracket__name__slash;
							}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space+ (AttrName Space* '=' Space* AttrValue)* AttrName Space+
						case state_type::open_abracket__name__attrname__space:
							if (is_space(c)) {}
							else if (is_starting_name(c))
							{
								pairs1.push_back(std::make_pair(std::move(param1), std::string("")));
								param1 = std::string();
								param1.push_back(c);
								state = state_type::open_abracket__name__attrname;
							}
							else if (is_assignment(c)) {state = state_type::open_abracket__name__attrequal;}
							else if (c == '>')
							{
								pairs1.push_back(std::make_pair(std::move(param1), std::string("")));
								state = state_type::closed_tag;
							}
							else if (is_slash(c))
							{
								pairs1.push_back(std::make_pair(std::move(param1), std::string("")));
								state = state_type::open_abracket__name__slash;
							}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space+ (AttrName Space* '=' Space* AttrValue)* AttrName Space* '=' Space*
						case state_type::open_abracket__name__attrequal:
							if (is_space(c)) {}
							else if (is_double_quote(c)) {state = state_type::open_abracket__name__dq;}
							else if (is_single_quote(c)) {state = state_type::open_abracket__name__sq;}
							else if (is_unquoted_attribute_value(c) || allowing_unquoted_attribute_value)
							{
								param3.push_back(c);
								state = state_type::open_abracket__name__uq;
							}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name Space+ (AttrName Space* '=' Space* AttrValue)* AttrName Space* '=' Space* '"' Value*
						case state_type::open_abracket__name__dq:
							if (is_double_quote(c)) state = state_type::open_abracket__name__attrend;
							else if (!is_open_abracket(c) || allowing_weak_double_quote_coding) {param3.push_back(c);}
							else {error = true;}
							break;

						// state so far is:
						// '<' Name (Space+ AttrName Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' Space* ''' Value*
						case state_type::open_abracket__name__sq:
							if (is_single_quote(c)) state = state_type::open_abracket__name__attrend;
							else if (!is_open_abracket(c) || allowing_weak_single_quote_coding) {param3.push_back(c);}
							else {error = true;}
							break;
						
						// state so far is:
						// '<' Name Space+ (AttrName Space* '=' Space* AttrValue)* AttrName Space* '=' Space* Value*
						case state_type::open_abracket__name__uq:
							if (is_space(c))
							{
								pairs1.push_back(std::make_pair(std::move(param2), std::move(param3)));
								param2 = std::string();
								param3 = std::string();
								state = state_type::open_abracket__name__space;
							}
							else if (is_closed_abracket(c))
							{
								pairs1.push_back(std::make_pair(std::move(param2), std::move(param3)));
								state = state_type::open_tag;
							}
							else if (is_slash(c))
							{
								pairs1.push_back(std::make_pair(std::move(param2), std::move(param3)));
								param2 = std::string();
								param3 = std::string();
								state = state_type::open_abracket__name__slash;
							}
							else if (is_unquoted_attribute_value(c)) param3.push_back(c);
							else {error = true;}
							break;

						// state so far is:
						// '<' Name (Space+ Attrname Space* '=' Space* AttrValue)* Space+ AttrName Space* '=' Space* AttrValue
						case state_type::open_abracket__name__attrend:
							pairs1.push_back(std::make_pair(std::move(param2), std::move(param3)));
							param2 = std::string();
							param3 = std::string();
							if (is_space(c)) state = state_type::open_abracket__name__space;
							else if (is_slash(c)) state = state_type::open_abracket__name__slash;
							else if (is_closed_abracket(c)) state = state_type::open_tag;
							else if (is_starting_name(c) && allowing_concated_attribute)
							{
								param2.push_back(c);
								state = state_type::open_abracket__name__attrname;
							}
							else {error = true;}
							break;

						// state so far is:
						// (Char\{'<'})+
						case state_type::characters:
							if (is_open_abracket(c)) state = state_type::text;
							else if (!skipping_text) {param1.push_back(c);}
							break;

						default:
							{assert(false);}
					}

					if (c == '\n') {column = 0; ++line;}
					else ++column;

					// in case of text we don't go to the next position since
					// we already have read some character necessary for reentering the loop
					// so we must hold the position of the iterator
					if (state != state_type::text) ++iter;
				}

				if (error)
					return std::make_tuple(iter, make_unknown_tag_token(formulate_error(state)+" at "+std::to_string(line)+","+std::to_string(column)));
				else if (state == state_type::text || state == state_type::initial || state == state_type::characters)
					return std::make_tuple(iter, make_text_token(std::move(param1)));
				else if (state == state_type::open_tag) return std::make_tuple(iter, make_open_tag_token(std::move(param1), std::move(pairs1)));
				else if (state == state_type::closed_tag) return std::make_tuple(iter, make_closing_tag_token(std::move(param1)));
				else if (state == state_type::empty_tag) return std::make_tuple(iter, make_empty_tag_token(std::move(param1), std::move(pairs1)));
				else if (state == state_type::process_instruction) return std::make_tuple(iter, make_pi_token(std::move(param1), std::move(param2)));
				else if (state == state_type::cdata) return std::make_tuple(iter, make_cdata_token(std::move(param1)));
				else if (state == state_type::dtd) return std::make_tuple(iter, make_dtd_token(std::move(param1)));
				else if (state == state_type::comment) return std::make_tuple(iter, make_comment_token(std::move(param1)));
				else return std::make_tuple(start, make_unknown_tag_token(std::string("reached end before entity were acceptely parsed!")));
			}

			template <typename InputIterator, typename AcceptId>
			std::tuple<InputIterator, std::string> parse_until_closing_tag(InputIterator begin, InputIterator end, AcceptId acceptId, size_t & line, size_t & column)
			{
				std::string param1;
				std::string param2;
				std::string param3;
				auto state = script_state::reading_script;

				while (begin != end && state != script_state::closing_tag)
				{
					auto c = *begin;
					
					switch (state)
					{
						case script_state::reading_script:
							if (c == '<') {state = script_state::open_abracket;}
							else {param1.push_back(c);}
							break;
						
						case script_state::open_abracket:
							if (c == '/') {state = script_state::open_abracket__slash;}
							else
							{
								param1.push_back('<');
								param1.push_back(c);
								state = script_state::reading_script;
							}
							break;

						case script_state::open_abracket__slash:
							if (is_starting_name(c)) {param2.push_back(c); state = script_state::open_abracket__slash__name;}
							else
							{
								param1.push_back('<');
								param1.push_back('/');
								param1.push_back(c);
								state = script_state::reading_script;
							}
							break;

						case script_state::open_abracket__slash__name:
							if (is_name(c)) {param2.push_back(c);}
							else if (is_space(c)) {param3.push_back(c); state = script_state::open_abracket__slash__name__space;}
							else if (c == '>' && acceptId(param2)) {param2.clear(); state = script_state::closing_tag;}
							else
							{
								param1.push_back('<');
								param1.push_back('/');
								std::for_each(param2.cbegin(), param2.cend(), [&param1](const char c){param1.push_back(c);});
								param2.clear();
								param1.push_back(c);
								state = script_state::reading_script;
							}
							break;

						case script_state::open_abracket__slash__name__space:
							if (is_space(c)) {param3.push_back(c);}
							else if (c == '>' && acceptId(param2)) {param2.clear(); param3.clear(); state = script_state::closing_tag;}
							else
							{
								param1.push_back('<');
								param1.push_back('/');
								std::for_each(param2.cbegin(), param2.cend(), [&param1](const char c){param1.push_back(c);});
								std::for_each(param3.cbegin(), param3.cend(), [&param1](const char c){param1.push_back(c);});
								param2.clear();
								param3.clear();
								param1.push_back(c);
								state = script_state::reading_script;
							}
							break;

						case script_state::closing_tag:
						default:
							assert(false);
					}

					if (c == '\n') {++line; column = 0;}
					else ++column;
					++begin;
				}
				std::for_each(param2.cbegin(), param2.cend(), [&param1](const char c){param1.push_back(c);});
				std::for_each(param3.cbegin(), param3.cend(), [&param1](const char c){param1.push_back(c);});
				return std::make_tuple(begin, std::move(param1));
			}

	};

}

#endif

