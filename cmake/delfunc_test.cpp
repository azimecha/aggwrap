class TestClass {
public:
	int x;
	TestClass(void) { x = 0; }
	
private:
	TestClass(const TestClass& robj) = delete;
	TestClass& operator=(const TestClass& robj) = delete;
};

int main(int argc, char** argv) {
	TestClass obj;
	return obj.x;
}
