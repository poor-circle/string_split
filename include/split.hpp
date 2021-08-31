#include<regex>
namespace mysplit
{

    //helper function to type-deduction
    namespace detail
    {
        template<class T, class...Args>
        auto test_callable_with_emplace_back(int) ->
            decltype(void(std::declval<T>().emplace_back(std::declval<Args>()...)), std::true_type{});
        template<class, class...>
        auto test_callable_with_emplace_back(...) -> std::false_type;

        template<class T, class...Args>
        auto test_callable_with_emplace(int) ->
            decltype(void(std::declval<T>().emplace(std::declval<Args>()...)), std::true_type{});
        template<class, class...>
        auto test_callable_with_emplace(...) -> std::false_type;

        template<class T, class...Args>
        auto test_callable_with_emplace_after(int) ->
            decltype(void(std::declval<T>().emplace_after(std::declval<Args>()...)), std::true_type{});
        template<class, class...>
        auto test_callable_with_emplace_after(...) -> std::false_type;

        template<class T, class...Args>
        struct callable_with_emplace_back :
        decltype(test_callable_with_emplace_back<T, Args...>(0)) {};

        template<class T, class...Args>
        struct callable_with_emplace :
        decltype(test_callable_with_emplace<T, Args...>(0)) {};

        template<class T, class...Args>
        struct callable_with_emplace_after :
        decltype(test_callable_with_emplace_after<T, Args...>(0)) {};

        template <class...> constexpr std::false_type always_false{};

    } // namespace detail
    
    //helper function to get singleton re-compiled regex
    template<typename char_type>
    decltype(auto) inline get_ws_regex()
    {
        if constexpr (std::is_same_v<char_type,char>)
        {
            const static auto ws_re=std::basic_regex<char_type>(R"(\s+)",std::regex::optimize);
            return ws_re;
        }
        else if constexpr (std::is_same_v<char_type,wchar_t>)
        {
            const static auto ws_re=std::basic_regex<char_type>(LR"(\s+)",std::regex::optimize);
            return ws_re;
        }
        else
        {
            static_assert(detail::always_false<char_type>,"c++ std regex lib only support char and wchar_t");
            const static auto ws_re=std::basic_regex<char_type>(R"(\s+)",std::regex::optimize);
            return ws_re;
        }
    }

    //split a string
    template<typename container_type,typename Iter>
    auto split(Iter begin,Iter end,const std::basic_regex<std::remove_const_t<std::remove_reference_t<decltype(*Iter{})>>>& r=get_ws_regex<std::remove_const_t<std::remove_reference_t<decltype(*Iter{})>>>())
    {
        container_type container;
        //if container support emplace_back(std::vector,std::deque,std::list,etc)
        constexpr bool support_emplace_back=detail::callable_with_emplace_back<container_type,typename container_type::value_type>::value;
        //if container support emplace(std::set,std::multiset,etc)
        constexpr bool support_emplace=detail::callable_with_emplace<container_type,typename container_type::value_type>::value;
        //if container support emplace_after(std::forward_list,etc)
        constexpr bool support_emplace_after=detail::callable_with_emplace_after<container_type,typename container_type::const_iterator,typename container_type::value_type>::value;
        
        if constexpr (support_emplace_back)
        {
            for (auto iter_begin=std::regex_token_iterator(begin,end,r,-1),iter_end=std::regex_token_iterator<Iter>();iter_begin!=iter_end;++iter_begin)
                if (iter_begin->length())
                    container.emplace_back(iter_begin->first,iter_begin->second);
        }
        else if constexpr (support_emplace)
        {
            for (auto iter_begin=std::regex_token_iterator(begin,end,r,-1),iter_end=std::regex_token_iterator<Iter>();iter_begin!=iter_end;++iter_begin)
                if (iter_begin->length())
                    container.emplace(iter_begin->first,iter_begin->second);
        }
        else if constexpr (support_emplace_after)
        {
            auto container_iter=container.before_begin();
            for (auto iter_begin=std::regex_token_iterator(begin,end,r,-1),iter_end=std::regex_token_iterator<Iter>();iter_begin!=iter_end;++iter_begin)
                if (iter_begin->length())
                    container_iter=container.emplace_after(container_iter,iter_begin->first,iter_begin->second);
        }
        else
            static_assert(detail::always_false<container>,"you should use a STL-like container(support emplace_back, emplace or emplace_after)");
        return container;  //NRVO
    }

    //split a string and save it to existed container
    template<typename Iter,typename Inserter_type>
    void split_to(Iter begin,Iter end,Inserter_type&& inserter,const std::basic_regex<std::remove_const_t<std::remove_reference_t<decltype(*Iter{})>>>& r=get_ws_regex<std::remove_const_t<std::remove_reference_t<decltype(*Iter{})>>>())
    {
        for (auto iter=std::regex_token_iterator(begin, end, r, -1),enditer=decltype(iter){};iter!=enditer;++iter)
            if (iter->first!=iter->second) 
                inserter={iter->first,iter->second};
    }


    //overload split function to make it use more comfortable
    template<typename container_type>
    auto split(const std::string_view str, const std::regex& r=get_ws_regex<char>()) {return split<container_type>(std::cbegin(str),std::cend(str),r);}

    template<typename container_type>
    auto split(const std::wstring_view str, const std::wregex& r=get_ws_regex<wchar_t>()) {return split<container_type>(std::cbegin(str),std::cend(str),r);}

    template<typename container_type>
    auto split(const char* str, const std::regex& r=get_ws_regex<char>()) {return split<container_type>(str,str+strlen(str),r);}

    template<typename container_type>
    auto split(const wchar_t* str, const std::wregex& r=get_ws_regex<wchar_t>()) {return split<container_type>(str,str+wcslen(str),r);}

    //overload split_to function to make it use more comfortable
    template<typename Inserter_type>
    void split_to(const std::string_view str,Inserter_type&& inserter,const std::regex& r=get_ws_regex<char>()) {split_to(std::cbegin(str),std::cend(str),inserter,r);}

    template<typename Inserter_type>
    void split_to(const std::wstring_view str,Inserter_type&& inserter,const std::wregex& r=get_ws_regex<wchar_t>()) {split_to(std::cbegin(str),std::cend(str),inserter,r);}

    template<typename Inserter_type>
    void split_to(const char* str,Inserter_type&& inserter,const std::regex& r=get_ws_regex<char>()) {split_to(str,str+strlen(str),inserter,r);}

    template<typename Inserter_type>
    void split_to(const wchar_t* str,Inserter_type&& inserter,const std::regex& r=get_ws_regex<wchar_t>()) {split_to(str,str+wcslen(str),inserter,r);}

}
