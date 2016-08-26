#pragma once
#ifndef __ZNEXPRESS_HPP_2015_09_08
#define __ZNEXPRESS_HPP_2015_09_08

///////////////////////////////////////////////////////////
//
//          ���ʽ������ (���Ի�����VS2010 SP1)
//			Powered By Tony
//			
//
//  ˵����
//    ��׼�������ⲻ֧��(?<=)��(?!=) 
// 	  �⵼����ȡ�Ӽ��ź�����������ʱ�Ҳ�֪����ô���ˣ�������ʱ���Ҫ��boost��������
//    ����㲻��Ҫ���������Ż����ܹ�д����ȡ�����ŵ�������ʽҲ����ֱ���ñ�׼��������
//    ͨ��_ZNEXPRESS_USE_BOOST ��ҵ�����Ƿ�ʹ��boost�������
//    ��Ҫ��չ���½��Լ���value��factory��operator��
//
//
//  һ�����������ʹ�÷���ʾ����
// 
//   ����ͷ�ļ�ǰʹ����������꣬ʹ��boost���������֧��(?<=)
// #define _ZNEXPRESS_USE_BOOST
//   Ҫ����ı��ʽ
// const ZnExpress::t_zn_char sExp[] = _T("(5+(+3--2)*-3)/2");
//   ����Ĭ�ϵ��������ʽת������ָ����ȡԪ�ص�������ʽ�����ʽԪ��Ϊ: ( ) + - * / �ʹ����ŵ�����
// ZnExpress::its_integer its(_T("\\(|\\)|(?<=\\d)\\+|(?<=\\d)-|\\*|/|[-+]?\\d+"));
//   �����ʽת��Ϊ��׺���ʽ
// ZnExpress::its_integer::t_sp_suffix spSuffix = its.to_suffix_sptr(sExp);
//   �õ����Ԫ��
// ZnExpress::its_integer::t_sp sp = spSuffix->result();
//   ���ֵ
// std::cout << sExp << " = " << static_cast<ZnExpress::its_integer::t_value*>(sp.get())->result() << std::endl;
//
//  ��������������ķ���
// ZnExpress::its_float its(_T("\\(|\\)|(?<=\\d)\\+|(?<=\\d)-|\\*|/|[-+]?\\d+\\.?\\d*"););
// const ZnExpress::t_zn_char sExp[] = _T("5.5+4.2/2.5*3.9-5.2");
// ZnExpress::its_float::t_sp_suffix spSuffix = its.to_suffix_sptr(sExp);
// ZnExpress::its_float::t_sp sp = spSuffix->result();
// std::cout << sExp << " = " << static_cast<ZnExpress::its_float::t_value*>(sp.get())->result() << std::endl;
//
///////////////////////////////////////////////////////////

// ʹ��boost��������
//#define _ZNEXPRESS_USE_BOOST

#include <vector>
#include <string>
#include <stack>
#include <memory>

#ifdef _ZNEXPRESS_USE_BOOST
#  include <boost/regex.hpp>
#else
#  include <regex>
#endif

namespace ZnExpress
{
#ifdef _UNICODE
	typedef wchar_t								t_zn_char;
	typedef std::wstring						t_zn_string;
#	define ZN_ATOI								_wtoi
#	define ZN_ATOF								_wtof
#	ifdef _ZNEXPRESS_USE_BOOST
	typedef boost::wregex						t_zn_regex;
	typedef boost::wsmatch						t_zn_match;
	typedef boost::wcregex_token_iterator		t_zn_regex_token_iter;
#	else
	typedef std::tr1::wregex					t_zn_regex;
	typedef std::tr1::wsmatch					t_zn_match;
	typedef std::tr1::wcregex_token_iterator	t_zn_regex_token_iter;
#	endif
#else
	typedef char								t_zn_char;
	typedef std::string							t_zn_string;
#	define ZN_ATOI								atoi
#	define ZN_ATOF								atof
#	ifdef _ZNEXPRESS_USE_BOOST
	typedef boost::regex						t_zn_regex;
	typedef boost::smatch						t_zn_match;
	typedef boost::cregex_token_iterator		t_zn_regex_token_iter;
#	else
	typedef std::tr1::regex						t_zn_regex;
	typedef std::tr1::smatch					t_zn_match;
	typedef std::tr1::cregex_token_iterator		t_zn_regex_token_iter;
#	endif
#endif
	// �쳣
	class exception
	{
	public:
		exception(const t_zn_char* str) : str_(str){}
		const t_zn_char* what() const {return str_.c_str();}
	private:
		t_zn_string	str_;
	};
	// ���ʽԪ�ػ���
	interface base_eliment
	{
		typedef std::tr1::shared_ptr<base_eliment>	t_sp;
		typedef std::stack<t_sp>					t_stack;
		~base_eliment(){}
		enum t_eliment_type
		{
			eliment_type_value = 0,
			eliment_type_left_bracket,
			eliment_type_right_bracket,
			eliment_type_operator,
		};
		virtual t_eliment_type	type() const = 0;			// ����
	};
	// ֵ����
	interface base_value : public base_eliment
	{
		t_eliment_type	type() const {return eliment_type_value;}
	};
	// ����������
	interface base_operator : public base_eliment
	{

		~base_operator(){}
		t_eliment_type			type() const {return eliment_type_operator;}
		virtual int				priority() const = 0;					// ����������ȼ�
		virtual t_sp			calculation(t_stack& stack_value) = 0;	// ����
	};
	// ��׺���ʽ��
	class suffix
	{
	public:
		typedef base_eliment::t_sp				t_sp;
		typedef base_eliment::t_stack			t_stack;
		typedef std::vector<t_sp>				t_vector;
		typedef std::tr1::shared_ptr<suffix>	t_sp_suffix;
		suffix()
			: is_calculation_(false)
		{}
		void set_suffix(const t_vector& vector_suffix){ vector_suffix_ = vector_suffix; }
		t_vector get_suffix() const {return vector_suffix_;}
		void swap_suffix(t_vector& vector_suffix){ vector_suffix_.swap(vector_suffix); }
		bool is_calculation() const {return is_calculation_;}
		void invalidate(){is_calculation_ = false;}
		t_sp result()
		{
			if (!is_calculation_)
			{
				t_stack stack_value;
				for (auto it = vector_suffix_.begin(); it != vector_suffix_.end(); ++it)
				{
					t_sp sp = *it;
					switch (sp->type())
					{
					case base_eliment::eliment_type_value:
						stack_value.push(sp);
						break;
					case base_eliment::eliment_type_operator:
						stack_value.push(static_cast<base_operator*>(sp.get())->calculation(stack_value));
						break;
					}
				}
				is_calculation_ = true;
				sp_result_ = stack_value.top();
			}
			return sp_result_;
		}
	private:
		t_vector	vector_suffix_;
		t_sp		sp_result_;
		bool		is_calculation_;
	};
	// ������
	class left_bracket : public base_eliment
	{
	public:
		t_eliment_type type() const{ return eliment_type_left_bracket;}
	};
	// ������
	class right_bracket : public base_eliment
	{
	public:
		t_eliment_type type() const{ return eliment_type_right_bracket;}
	};
	// ��׺���ʽ�ַ���ת��Ϊ��׺���ʽ
	// ���캯����keyword�ַ���Ϊ���ʽ����Ԫ����ȡ��������ʽ
	template<typename t_value, typename t_factory>
	class infix_to_suffix
	{
	public:
		// ���Ͷ���
		typedef typename suffix						t_suffix;
		typedef t_value								t_value;
		typedef t_factory							t_factory;
		typedef std::tr1::shared_ptr<t_suffix>		t_sp_suffix;
		typedef typename t_suffix::t_sp				t_sp;
		typedef typename t_suffix::t_stack			t_stack;
		typedef typename t_suffix::t_vector			t_vector;
	public:
		infix_to_suffix(const t_zn_char* keyword) : regex_(keyword){}
		t_suffix to_suffix(const t_zn_char* exp)
		{
			t_vector vec_infix, vec_suffix;
			split_eliment_(exp, vec_infix);
			if (vec_infix.empty()) throw exception(_T("���ʽ��������δ�������κα��ʽԪ��"));
			conversion_(vec_infix, vec_suffix); 
			t_suffix suffix;
			suffix.swap_suffix(vec_suffix);
			return std::move(suffix);
		}
		t_sp_suffix to_suffix_sptr(const t_zn_char* exp)
		{
			t_vector vec_infix, vec_suffix;
			split_eliment_(exp, vec_infix);
			if (vec_infix.empty()) throw exception(_T("���ʽ��������δ�������κα��ʽԪ��"));
			conversion_(vec_infix, vec_suffix); 
			t_sp_suffix sp_suffix(new t_suffix);
			sp_suffix->swap_suffix(vec_suffix);
			return sp_suffix;
		}
	private:
		void split_eliment_(const t_zn_char* exp, t_vector& vec_infix)
		{
			// ���а汾ʹ��stringЧ�ʸ��ߣ����ǵ��԰汾ʹ��string̫����
#		ifdef _DEBUG
			split_eliment_use_t_char_(exp, vec_infix);
#		else 
			split_eliment_use_string_(exp, vec_infix);
#		endif
		}
		void split_eliment_use_t_char_(const t_zn_char* exp, t_vector& vec_infix)
		{
			t_factory creator;
			t_zn_regex_token_iter end;
			for (t_zn_regex_token_iter it(exp, exp + _tcslen(exp), regex_); it != end; ++it)
				vec_infix.push_back(creator.create(it->str().c_str()));
		}
		void split_eliment_use_string_(const t_zn_char* exp, t_vector& vec_infix)
		{
			t_zn_string str(exp);
			t_zn_match	matchs;
			t_factory creator;
#		ifdef _ZNEXPRESS_USE_BOOST
			while(boost::regex_search(str, matchs, regex_))
#		else
			while(std::tr1::regex_search(str, matchs, regex_))
#		endif
			{
				vec_infix.push_back(creator.create(matchs.str().c_str()));
				str.swap(matchs.suffix().str());
			}
		}
		void conversion_(t_vector& vec_infix, t_vector& vec_suffix)
		{
			t_stack stack_operator;
			for (t_vector::iterator it = vec_infix.begin(); it != vec_infix.end(); ++it)
				conversion_element_(*it, vec_suffix, stack_operator);
			push_back_stack_surplus_to_suffix_(vec_suffix, stack_operator);
		}
		void conversion_element_(t_sp sp, t_vector& vec_suffix, t_stack& stack_operator)
		{
			switch (sp->type())
			{
				// ������ֱ�ӷ����׺����
			case base_eliment::eliment_type_value:
				conversion_value_(sp, vec_suffix, stack_operator);
				break;
				// ��������ջ
			case base_eliment::eliment_type_left_bracket:
				conversion_left_bracket_(sp, vec_suffix, stack_operator);
				break;
				// �����Ž��������е����в�����ջ
			case base_eliment::eliment_type_right_bracket:
				conversion_right_bracket_(sp, vec_suffix, stack_operator);
				break;
				// ������
			case base_eliment::eliment_type_operator:
				conversion_operator_(sp, vec_suffix, stack_operator);
				break;
			}
		}
		void conversion_value_(t_sp sp, t_vector& vec_suffix, t_stack& stack_operator)
		{
			vec_suffix.push_back(sp);
		}
		void conversion_left_bracket_(t_sp sp, t_vector& vec_suffix, t_stack& stack_operator)
		{
			stack_operator.push(sp);
		}
		void conversion_right_bracket_(t_sp sp, t_vector& vec_suffix, t_stack& stack_operator)
		{
			if (stack_operator.empty()) throw exception(_T("���Ų�ƥ�䣬ȱ�� '('"));
			while(stack_operator.top()->type() != base_eliment::eliment_type_left_bracket)
			{
				vec_suffix.push_back(stack_operator.top());
				stack_operator.pop();
				if (stack_operator.empty()) throw exception(_T("���Ų�ƥ�䣬ȱ�� '('"));
			}
			stack_operator.pop();
		}
		void conversion_operator_(t_sp sp, t_vector& vec_suffix, t_stack& stack_operator)
		{
			// ���ջΪ������ջ
			if (stack_operator.empty())
				stack_operator.push(sp);
			// �����������ȼ�����ջ�������ȼ�����ջ
			else if (operator_priority_(sp) > operator_priority_(stack_operator.top()))
				stack_operator.push(sp);
			// ���ջ��Ϊ�ղ���ջ����������Ϊ���ţ������׺������ֱ��continue
			else if (!stack_operator.empty() && stack_operator.top()->type() != base_eliment::eliment_type_left_bracket)
			{
				vec_suffix.push_back(stack_operator.top());
				stack_operator.pop();
				conversion_element_(sp, vec_suffix, stack_operator);
			}
			// ���������ջ
			else
				stack_operator.push(sp);
		}
		int	 operator_priority_(t_sp sp)
		{
			if (sp->type() != base_eliment::eliment_type_operator) return -1;
			return static_cast<base_operator*>(sp.get())->priority();
		}
		void push_back_stack_surplus_to_suffix_(t_vector& vec_suffix, t_stack& stack_operator)
		{
			while (!stack_operator.empty())
			{
				t_sp sp = stack_operator.top();
				if (sp->type() == base_eliment::eliment_type_left_bracket) throw exception(_T("���Ų�ƥ�䣬���ڵ�'('"));
				vec_suffix.push_back(sp);
				stack_operator.pop();
			}
		}
	private:
		t_zn_regex		regex_;
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////
	//////   ��������
	//////   + - * / =

	// ��
	template<typename t_value, int n_priority>
	class operator_addition : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'+'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'+'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) + *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ��
	template<typename t_value, int n_priority>
	class operator_subtraction : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'-'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'-'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) - *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ��
	template<typename t_value, int n_priority>
	class operator_multiplication : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'*'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'*'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) * *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ��
	template<typename t_value, int n_priority>
	class operator_division : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'/'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'/'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) / *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ��ֵ
	template<typename t_value, int n_priority>
	class operator_assignment : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'='�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'='�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) = *static_cast<t_value*>(sp_right.get())));
		}
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////
	//////   ��ϵ����
	//////   > >= < <= != == 

	// ����
	template<typename t_value, int n_priority>
	class operator_gtr : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'>'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'>'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) > *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ���ڵ���
	template<typename t_value, int n_priority>
	class operator_geq : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'>='�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'>='�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) >= *static_cast<t_value*>(sp_right.get())));
		}
	};
	// С��
	template<typename t_value, int n_priority>
	class operator_lss : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'<'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'<'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) < *static_cast<t_value*>(sp_right.get())));
		}
	};
	// С�ڵ���
	template<typename t_value, int n_priority>
	class operator_leq : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'<='�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'<='�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) <= *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ������
	template<typename t_value, int n_priority>
	class operator_neq : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'!='�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'!='�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) != *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ����
	template<typename t_value, int n_priority>
	class operator_equ : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'=='�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'=='�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) == *static_cast<t_value*>(sp_right.get())));
		}
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////
	//////   �߼�����
	//////   ! && || 

	// ��
	template<typename t_value, int n_priority>
	class operator_not : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'!'�����ȱ�ٲ���"));
			t_sp sp = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(!*static_cast<t_value*>(sp.get())));
		}
	};
	// ��
	template<typename t_value, int n_priority>
	class operator_and : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'&&'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'&&'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) && *static_cast<t_value*>(sp_right.get())));
		}
	};
	// ��
	template<typename t_value, int n_priority>
	class operator_or : public base_operator
	{
	public:
		virtual int				priority() const {return n_priority;}
		virtual t_sp			calculation(t_stack& stack_value)
		{
			if (stack_value.empty()) throw exception(_T("'||'�����ȱ�ٲ���"));
			t_sp sp_right = stack_value.top(); stack_value.pop();
			if (stack_value.empty()) throw exception(_T("'||'�����ȱ�ٲ���"));
			t_sp sp_left = stack_value.top(); stack_value.pop();
			return t_sp(new t_value(*static_cast<t_value*>(sp_left.get()) || *static_cast<t_value*>(sp_right.get())));
		}
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////
	//////   һЩ����Ĭ�ϵı��ʽ����
	//////   

	// ����ֵ
	class value_integer : public base_value
	{
	public:
		value_integer(int value) : value_(value){}
		value_integer(const t_zn_char* str_ptr) : value_(ZN_ATOI(str_ptr)){}
		int result() const {return value_;}
		int operator +  (const value_integer& obj) { return result() + obj.result(); }
		int operator -  (const value_integer& obj) { return result() - obj.result(); }
		int operator *  (const value_integer& obj) { return result() * obj.result(); }
		int operator /  (const value_integer& obj) { return result() / obj.result(); }
		int operator >  (const value_integer& obj) { return (result() >  obj.result()) ? 1 : 0; }
		int operator >= (const value_integer& obj) { return (result() >= obj.result()) ? 1 : 0; }
		int operator <  (const value_integer& obj) { return (result() <  obj.result()) ? 1 : 0; }
		int operator <= (const value_integer& obj) { return (result() <= obj.result()) ? 1 : 0; }
		int operator != (const value_integer& obj) { return (operator ==(obj)) ? 0 : 1; }
		int operator == (const value_integer& obj) { return (result() == obj.result()) ? 1 : 0; }
		int operator && (const value_integer& obj) { return (result() && obj.result()) ? 1 : 0; }
		int operator || (const value_integer& obj) { return (result() || obj.result()) ? 1 : 0; }
		int operator !  () { return result() == 0 ? 1 : 0; }
	private:
		int	value_;
	};
	// ������
	class value_float : public base_value
	{
	public:
		value_float(double value) : value_(value), precision_(0.00001){}
		value_float(const t_zn_char* str_ptr) : value_(ZN_ATOF(str_ptr)) , precision_(0.00001){}
		double result() const {return value_;}
		double operator +  (const value_float& obj) { return result() + obj.result(); }
		double operator -  (const value_float& obj) { return result() - obj.result(); }
		double operator *  (const value_float& obj) { return result() * obj.result(); }
		double operator /  (const value_float& obj) { return result() / obj.result(); }
		double operator >  (const value_float& obj) { return (result() >  obj.result()) ? 1 : 0; }
		double operator >= (const value_float& obj) { return (result() >= obj.result()) ? 1 : 0; }
		double operator <  (const value_float& obj) { return (result() <  obj.result()) ? 1 : 0; }
		double operator <= (const value_float& obj) { return (result() <= obj.result()) ? 1 : 0; }
		double operator != (const value_float& obj) { return !operator ==(obj); }
		double operator == (const value_float& obj) { return (fabs(result() - obj.result()) < precision_) ? 1 : 0; }
		double operator && (const value_float& obj) { return (result() && obj.result()) ? 1 : 0; }
		double operator || (const value_float& obj) { return (result() || obj.result()) ? 1 : 0; }
		double operator !  () { return (fabs(result() - 0) < precision_) ? 1 : 0; }
	private:
		double			value_;
		const double	precision_;
	};
	// Ĭ�ϵĹ���
	template<typename t_value>
	class factory_default
	{
	public:
		typedef typename base_eliment::t_sp t_sp;
		t_sp create(const t_zn_char* str_ptr)
		{
			if (_tcscmp(str_ptr, _T("(")) == 0) return t_sp(new left_bracket);
			else if (_tcscmp(str_ptr, _T(")")) == 0) return t_sp(new right_bracket);
			else if (_tcscmp(str_ptr, _T("*")) == 0) return t_sp(new operator_multiplication<t_value, 8>);
			else if (_tcscmp(str_ptr, _T("/")) == 0) return t_sp(new operator_division<t_value, 8>);
			else if (_tcscmp(str_ptr, _T("+")) == 0) return t_sp(new operator_addition<t_value, 7>);
			else if (_tcscmp(str_ptr, _T("-")) == 0) return t_sp(new operator_subtraction<t_value, 7>);
			else if (_tcscmp(str_ptr, _T(">")) == 0) return t_sp(new operator_gtr<t_value, 5>);
			else if (_tcscmp(str_ptr, _T(">=")) == 0) return t_sp(new operator_geq<t_value, 5>);
			else if (_tcscmp(str_ptr, _T("<")) == 0) return t_sp(new operator_lss<t_value, 5>);
			else if (_tcscmp(str_ptr, _T("<=")) == 0) return t_sp(new operator_leq<t_value, 5>);
			else if (_tcscmp(str_ptr, _T("!=")) == 0) return t_sp(new operator_neq<t_value, 5>);
			else if (_tcscmp(str_ptr, _T("==")) == 0) return t_sp(new operator_equ<t_value, 5>);
			else if (_tcscmp(str_ptr, _T("!")) == 0) return t_sp(new operator_not<t_value, 6>);
			else if (_tcscmp(str_ptr, _T("&&")) == 0) return t_sp(new operator_and<t_value, 3>);
			else if (_tcscmp(str_ptr, _T("||")) == 0) return t_sp(new operator_or<t_value, 2>);
			return t_sp(new t_value(str_ptr));
		}
	};

	typedef infix_to_suffix<value_integer, factory_default<value_integer> > its_integer;
	typedef infix_to_suffix<value_float, factory_default<value_float> >		its_float;
}

#endif