/*
 * @Author: Zezheng Li
 * @Date: 2021-08-31 23:10:51
 * @LastEditors: Zezheng Li
 * @LastEditTime: 2021-09-01 16:02:31
 * @Description: file content
 */
#include"include/split.hpp"

#include<iostream>
#include<string_view>
#include<string>
#include<set>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include<list>
#include<forward_list>
#include<deque>
#include<queue>
#include<stack>

using namespace std;
using namespace mysplit;
int main(void)
{
    string s;
    getline(cin,s);

    //default behavior: split by ws_re
    auto a1=split<vector<string_view>>(s);// a1 is a vector<string>

    //split by regex
    auto a2=split<vector<string_view>>(s,regex("[0-9]"));

    //save to any type of container
    auto a3=split<deque<string_view>>(s);
    auto a5=split<unordered_multiset<string>>(s,regex("[0-9]"));

    //save to string
    auto a4=split<forward_list<string>>(s);

    //save to existed container
    deque<string> a6({"11","22","33","44"});
    auto ins=inserter(a6,find(a6.begin(),a6.end(),"22"));//get an inserter that always insert after "22"
    split_to(s,ins);//insert before "22"
    split_to(s,back_inserter(a3));//insert at back
    split_to(s,front_inserter(a3));//insert at front


    //output by ostream_iterator
    split_to(s.begin(),s.end(),ostream_iterator<string_view>(cout,"\n"));
    //save to adapter
    queue a7(split<vector<string_view>>(s));
    stack a8(split<vector<string_view>>(s));

    //split from string's iterator
    auto a9=split<deque<string_view>>(s.begin(),s.end());

    //support split from string_view
    string_view sv=s;
    auto a10=split<list<string_view>>(sv.begin(),sv.end());
    auto a11=split<set<string_view>>(sv,regex("[0-9]"));
    

    //support const char *
    const char * sptr="hello world";
    auto a12=split<vector<string_view>>(sptr);
    auto a13=split<vector<string_view>>(sptr,sptr+8,regex("[0-9]"));

    //support const wchar_t *
    const wchar_t * wsptr=L"hello world";
    auto a14=split<vector<wstring_view>>(wsptr);
    auto a15=split<vector<wstring_view>>(wsptr,wsptr+8,wregex(L"[0-9]"));

    //support wstring
    auto wstr=L"hello world"s;
    auto a16=split<vector<wstring_view>>(wstr);
    auto a17=split<multiset<wstring>>(wstr,wregex(L"[0-9]"));

    //support wstring_view
    wstring_view wsv=wstr;
    auto a18=split<unordered_set<wstring_view>>(wsv);
    return 0;
}