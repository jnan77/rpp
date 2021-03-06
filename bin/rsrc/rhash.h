//简单的哈希表实现

template<typename T>
struct rhash_i
{
	T val;
	rstr name;
	rhash_i<T>* pre;
	rhash_i<T>* next;
};

template<typename T>
struct rhash
{
	rbuf<rhash_i<T>*> m_hash;
	int m_count;

	~rhash<T>()
	{
		clear();
	}

	rhash<T>()
	{
		init();
	}
	
	rhash<T>(rhash<T>& a)
	{
		init();
		for p=a.begin;p!=a.end;p=next(p)
			insert(*p)
	}
	
	operator=(rhash<T>& a)
	{
		clear();
		for p=a.begin;p!=a.end;p=a.next(p)
			insert(*p)
	}

	T& operator[](rstr name)
	{
		T* p=find(name);
		if(p)
		{
			return *p;
		}
		//找不到则插入一个空值
		T a;
		insert(a,name);
		p=find(name);
		return *p;
	}

	void init()
	{
		//两个字节消耗256K内存
		m_hash.realloc_n(65536);
		for(int i=0;i<m_hash.count();i++)
			m_hash[i]=null;
		m_count=0;
	}

	void clear()
	{
		if(m_count==0)
			return;
		for(int i=0;i<m_hash.count();i++)
		{
			if(m_hash[i]!=null)
			{
				rhash_i<T>* p=m_hash[i];
				while(p!=null)
				{
					rhash_i<T>* temp=p;
					p=p->next;
					r_delete<rhash_i<T>>temp;
				}
			}
		}
		init();
	}

	rbool exist(rstr name)
	{
		return find(name)!=null;
	}

	T* find(rstr name)
	{
		int addr=get_addr(name);
		rhash_i<T>* p=m_hash[addr];
		while(p)
		{
			if(p->name==name)
				return p;
			p=p->next;
		}
		return null;
	}
	
	//检测重复的插入
	void insert_c(T& a,rstr name="")
	{
		if(exist(name))
			return;
		insert(a,name);
	}
	
	//直接插入
	void insert(T& a,rstr name="")
	{
		int addr=get_addr(name);
		rhash_i<T>* p=r_new<rhash_i<T>>1;
		p->name=name;
		p->next=m_hash[addr];
		p->pre=null;
		p->val=a;
		if(m_hash[addr]!=null)
			m_hash[addr]->pre=p;
		m_hash[addr]=p;
		m_count++;
	}

	rbool erase(rstr name="")
	{
		rhash_i<T>* p=find(name);
		if(p==null)
			return false;
		if(p->pre!=null)
		{
			p->pre->next=p->next;
			if(p->next!=null)
				p->next->pre=p->pre;
		}
		else
		{
			m_hash[get_addr(name)]=p->next;
			if(p->next!=null)
				p->next->pre=null;
		}
		r_delete<rhash_i<T>>p;
		m_count--;
		return true;
	}

	T* begin()
	{
		for(int i=0;i<m_hash.count();i++)
			if(m_hash[i]!=null)
			{
				q=m_hash[i];
				return q;
			}
		return null;
	}
	
	T* end()
	{
		return null;
	}

	T* rend()
	{
		for(int i=m_hash.count()-1;i>=0;i--)
			if(m_hash[i]!=null)
			{
				rhash_i<T>* p=m_hash[i];
				while(p!=null)
				{
					if p->next==null
						break
					p=p->next;
				}
				return p;
			}
		return null;
	}
	
	//这个next有点慢，最好把一个桶的最后一个元素的next指向下一个桶
	T* next(T* cur)
	{
		rhash_i<T>* p=cur;
		if(p->next!=null)
		{
			return r_to_p<T*>p->next;
		}
		int addr=get_addr(p->name);
		for(int i=addr+1;i<m_hash.count();i++)
			if(m_hash[i]!=null)
			{
				q=m_hash[i];
				return q;
			}
		return null;
	}

	static int get_addr(rstr& name)
	{
		return bkdr_hash16(name.begin(),name.count());
	}

	static uint bkdr_hash32(char* data,int len)
	{
		uint seed=131; // 31 131 1313 13131 131313 etc..
		uint hash=0;
		for(int i=0;i<len;++i)
		{
			hash=(hash*seed)+data[i].toint;
		}
		return hash;
	}

	static int bkdr_hash16(char* data,int len)
	{
		uint val=bkdr_hash32(data,len);
		return (val%65536).toint;
	}
};
