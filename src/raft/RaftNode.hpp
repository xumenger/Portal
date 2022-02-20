/**
 * 2022.02.19 by xumenger
 *
 * 集群节点相关定义
 *
 */
#ifndef PORTAL_RAFT_NODE_CPP
#define PORTAL_RAFT_NODE_CPP

#include <iostream>

using namespace std;

namespace portal
{
namespace raft
{

    /**
     * NodeId只是字符串的一个封装
     * 没有直接使用字符串的原因是，假如之后有设计变更，
     * 可以不用到处寻找含有服务器节点ID 的字符串，直接修改NodeId 代码即可
     */
    class NodeId
    {
        private:
            string value;

        public:
            // 构造方法
            NodeId(string v)
            {
                this.value = v;
            }

            // 析构方法
            virtual ~NodeId()
            {
            }

            // http://www.xumenger.com/cpp-return-reference-20170916/
            // const 避免返回值被修改
            // & 表示引用方式返回，避免拷贝
            const string &getValue()
            {
                return this.value;
            }

            // ==运算符重载。参数使用&，引用传值，避免拷贝传值
            // C++ 的引用类型与Java 不同，一定不为NULL
            // C++ 中只有指针可能是NULL
            bool operator==(NodeId &n)
            {
                if (this.value == n.value)
                {
                    return true;
                }

                return false;
            }

            // !=运算符重载
            bool operator!=(NodeId &n)
            {
                return !(this == n);
            }
    };
}
}

#endif
