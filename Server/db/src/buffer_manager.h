#ifndef __INC_METIN_II_GAME_BUFFER_MANAGER_H__
#define __INC_METIN_II_GAME_BUFFER_MANAGER_H__

class TEMP_BUFFER
{
	public:
		TEMP_BUFFER(int Size = 8192, bool ForceDelete = false )
		{
			forceDelete = ForceDelete;

			if (forceDelete)
				Size = MAX(Size, 1024 * 128);

			buf = buffer_new(Size);
		}

		~TEMP_BUFFER()
		{
			buffer_delete(buf);
		}

		const void * 	read_peek() { return (buffer_read_peek(buf)); }
		void		write(const void * data, int size) { buffer_write(buf, data, size); }
		int		size() { return buffer_size(buf); }
		void	reset() { buffer_reset(buf); }

		LPBUFFER	getptr() { return buf; }

	protected:
		LPBUFFER	buf;
		bool		forceDelete;
};

#endif
