#ifndef __MESSAGE_QUEUE_CIRCLE_QUEUE_H__
#define __MESSAGE_QUEUE_CIRCLE_QUEUE_H__

#include <stdint.h>
//#include <inttypes.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include "common_define.h"

#define CIRCLEQUEUE_REVERSE_SIZE	64
class CCircleQueue
{
public:

	enum
	{
		invalid_offset = -1, /**< 无效的偏移量 */
		reserved_length = 8, /**< 预留的空间长度 */
		queue_buffer_is_not_enough = 2,		/**< 队列空间不够 */
		queue_is_empty			   = 3,		/**< 队列为空 */
		recv_buffer_is_not_enough  = 4,		/**< 接受队列空间不够 */
		queue_is_not_ini		   = 5,		/**< 队列未初始化 */
	};
	//bIgnoreScrap = true (断点) 表示如果尾端不够压入新的数据的时候,再回到头端,这样使用get的时候是一片连续的内存,就不需要拷贝了
	//bReserve = true表示在buffer内部空出了CIRCLEQUEUE_REVERSE_SIZE的空间用于记录队列自身的信息 
	CCircleQueue();
	CCircleQueue(void* pBuffer, uint32_t nLen, bool bIgnoreScrap = true, bool bReserve = true);
	~CCircleQueue(void);
	char* reset(void* pBuffer, uint32_t nLen, bool bIgnoreScrap = true, bool bReserve = true);

protected://attributes 
	//数据区总长度，单位: Byte。 
	volatile uint32_t m_iSize;
	//可用数据起始位置 
	volatile uint32_t m_iHead;
	//可用数据结束位置 
	volatile uint32_t m_iTail;
	//写入数据计数 
	volatile uint32_t m_iPush;
	//读取数据计数 
	volatile uint32_t m_iPop;
	//数据区的偏移位置 

	uint32_t m_iReserve;

	char*			m_pBuffer;
	const uint32_t	m_ciNodeHeadLen;
	bool m_bIgnoreScrap;

public:
	void initialize();
	/*
	* @method:    append 从尾部追加消息buffer。该函数只是改变m_iTail，即使是在多线程环境下也不需要加/解锁操作，
	*				因为，读写操作是原子操作
	:注解:其实读写操作并不是原子操作,c/c++到汇编会编成1或3条操作,但是一个线程只读,一个线程只写,是没有问题的
	* @fullname:  Game51::Server::CCodeQueue::append
	* @access:    public 
	* @param: const char * code
	* @param: int32_t size
	* @return:   int32_t。 
	* - 0: 成功
	* - >0 : 失败
	* @qualifier: 
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t append(const char* code, uint32_t size);


	/*
	* @method:    pop 从队列头部取一条消息,拷贝到dst地址
	* @fullname:  Game51::Server::CCodeQueue::pop
	* @access:    public 
	* @param[in]: char * dst
	* @param[in,out]: short & outlength。传入dst的长度，传出实际消息code的长度
	* @return:   int32_t
	* - 0: 成功. outlength返回实际code的长度
	* - >0 : 失败， dst,outlength的值无意义
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	int32_t pop(char* dst, uint32_t& outlength);

	//获取数据的内存,用户不用复制 
	const char* get(uint32_t& outlength);
	//和get结合,弹出最后一个数据 
	int32_t pop();
	//多线程没有上锁,多线程不要用这函数 
	uint32_t get_count();


	/*
	* @method:    pop_from
	* @fullname:  Game51::Server::CCodeQueue::pop_from
	* @access:    public 
	* @param[in]: int32_t offset 指定从offset处取code
	* @param[in]: int32_t codesize 指定获取的code长度
	* @param[in]: char * dst
	* @param[in,out]: int32_t & outlenght
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated  目前暂时不实现该方法，因为底层为连续存储的队列中取数据会造成数据移动，效率较低。
	*/
	int32_t pop_from(int32_t offset, int32_t codesize, char* dst, int32_t& outlenght);



	/*
	* @method:    full 判断队列是否满
	* @fullname:  Game51::Server::CCodeQueue::full
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	bool full(void);


	/*
	* @method:    empty 判读队列是否为空
	* @fullname:  Game51::Server::CCodeQueue::empty
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier:
	* @note	
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	bool empty(void);
	void clear(void);

protected:
	/** 返回数据区偏移 */
	int32_t code_offset(void)const;

	int32_t set_boundary(uint32_t head, uint32_t tail);
    int32_t set_head(uint32_t head);
    int32_t set_tail(uint32_t tail);

	int32_t get_boundary(uint32_t& head, uint32_t& tail);

	char* get_codebuffer(void)const;

	//1是不支持断点的,2是支持断点的 
	int32_t append1(const char* code, uint32_t size);
	int32_t append2(const char* code, uint32_t size);
	int32_t pop1(char* dst, uint32_t& outlength);
	int32_t pop2(char* dst, uint32_t& outlength);

public:
	/*
	* @method:    get_freesize
	* @fullname:  Game51::Server::CCodeQueue::get_freesize
	* @access:    protected 
	* @param: void
	* @return:   int32_t
	* - 返回可用空间的大小。如果没有可用空间，返回值为0。也就是说该函数的
	*	返回值总是>=0的。
	* @qualifier: const
	* @note	保证队列buffer的长度不能为0,否则结果为定义.
	* @par 示例:
	* @code

	* @endcode

	* @see
	* @deprecated 
	*/
	uint32_t get_freesize(void);

	uint32_t get_codesize(void);

};

#endif

