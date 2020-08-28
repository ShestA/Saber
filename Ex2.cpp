#include <string>
#include <list>
#include <algorithm>
#include <functional>
//Сериализация, десериализация
struct ListNode  {
	ListNode* rand{};
	std::string data;
};
class List : private std::list<ListNode*> {
public:
	void Serialize(FILE* file);
	void Deserialize(FILE* file);
	/*Тестовая функция*/
	void Push_back(std::string data);
	//Интерфейс для работы с модифицированным списком не был реализован, так как это выходит за рамки задания
private:
	/*Поиск индекса по адресу*/
	std::uint64_t FindN(const ListNode* value) const;
	/*Поиск адреса по индексу*/
	ListNode* FindRef(size_t N) const;
	/*Сериализация узла*/
	void NodeSerialize(FILE* file, const ListNode* in) const;
	/*Восстановление rand поля узла*/
	void NodeRecover(std::uint64_t* refs, ListNode* in);
};

/*
*	Структура выходного файла:
*	8 байт: кол-во узлов
*	Блок одного узла:
*	8 байт: номер случайного узла
*	8 байт: N байт данных узла
*	N байт: данные узла
*/
void List::Serialize(FILE* file){
	std::uint64_t len;

	len = static_cast<std::uint64_t>(this->size());
	if (len != 0) {
		fwrite(&len, sizeof(std::uint64_t), 1, file);
		std::function<void(ListNode*)> func = std::bind(&List::NodeSerialize, this, file, std::placeholders::_1);
		std::for_each(this->begin(), this->end(), func);
	}
}

void List::Deserialize(FILE* file) {
	std::uint64_t len;
	std::uint64_t* randref;
	std::uint64_t* buffer;
	char* dataBuffer;
	std::uint64_t dataLen;

	buffer = new std::uint64_t;
	*buffer = 0;
	fread(buffer, sizeof(std::uint64_t), 1, file);
	len = *buffer;
	if (len != 0) {
		randref = new std::uint64_t[len];
		for (size_t i = 0; i < len; i++) {
			/*
			*	Мне кажется, здесь можно написать красивее и 
			*	эффективнее, но ничего не приходит на ум
			*/
			fread(buffer, sizeof(std::uint64_t), 1, file);
			randref[i] = *buffer;
			fread(buffer, sizeof(std::uint64_t), 1, file);
			dataLen = *(buffer);
			dataBuffer = new char[dataLen + 1];
			fread(dataBuffer, dataLen, 1, file);
			dataBuffer[dataLen] = '\0';
			Push_back(std::string(dataBuffer));
			delete[] dataBuffer;
		}
		std::uint64_t* cursor;

		cursor = randref;
		std::function<void(ListNode*)> func = std::bind(&List::NodeRecover, this, std::cref(cursor), std::placeholders::_1);
		std::for_each(this->begin(), this->end(), [&func, &cursor](ListNode* val) {func(val); cursor++;});
		delete[] randref;
	}
	delete buffer;
}

void List::Push_back(std::string data) {
	ListNode* newNode;

	newNode = new ListNode;
	newNode->data = data;
	newNode->rand = this->back();
	/*
	*	Здесь можно реализовать случайный выбор узла из имеющихся, или еще что-то
	*/
	push_back(newNode);
}

void List::NodeSerialize(FILE* file, const ListNode* in) const {
	std::uint64_t len;
	std::uint64_t randref;

	randref = FindN(in->rand);
	len = static_cast<std::uint64_t>(in->data.size());
	fwrite(&randref, sizeof(std::uint64_t), 1, file);
	fwrite(&len, sizeof(std::uint64_t), 1, file);
	fwrite(in->data.c_str(), sizeof(char), in->data.size(), file);
}

void List::NodeRecover(std::uint64_t* ref, ListNode* in) {
	size_t N;
	
	N = *ref;
	in->rand = FindRef(N);
}

std::uint64_t List::FindN(const ListNode* value) const {
	auto itr = this->begin();
	std::uint64_t N = 1;

	if (value != nullptr) {
		do {
			if ((*itr) == value)
				return N;
			N++;
		} while (++itr != this->end());
	}

	return 0;
}

ListNode* List::FindRef(size_t N) const {
	auto itr = this->begin();

	if (N == 0) return nullptr;
	while (N-- > 1) itr++;

	return *itr;
}

int test() {
	List a, b;
	FILE* f;

	f = fopen("a.txt", "wb");
	a.Push_back("Hello");
	a.Push_back("I'm");
	a.Push_back("Aleksei");
	a.Push_back("Shestakov");
	a.Serialize(f);
	fclose(f);
	f = fopen("a.txt", "rb");
	b.Deserialize(f);
	fclose(f);
	f = fopen("b.txt", "wb");
	b.Serialize(f);
	fclose(f);

	return 0;
}