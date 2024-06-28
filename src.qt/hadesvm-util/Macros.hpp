//
//  hadesvm-util/Macros.cpp
//
//  Helper macros
//
//////////

#define HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Clazz)  \
    private:                                            \
        Clazz(const Clazz &) = delete;                  \
        Clazz(Clazz &&) = delete;                       \
        Clazz & operator = (const Clazz &) = delete;    \
        Clazz & operator = (Clazz &&) = delete;

#define HADESVM_UTILITY_CLASS(Clazz)                    \
    HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Clazz)      \
    private:                                            \
        Clazz() = delete;                               \
        ~Clazz() = delete;

#define HADESVM_DECLARE_SINGLETON(Clazz)                \
    HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Clazz)      \
    public:                                             \
        Clazz();                                        \
        ~Clazz();                                       \
    public:                                             \
        static Clazz *  instance();

#define HADESVM_IMPLEMENT_SINGLETON(Clazz)              \
    Clazz * Clazz::instance()                           \
    {                                                   \
        static Clazz theInstance;                       \
        return &theInstance;                            \
    }

//  We don't want some of the deprecated keywords
#define register        _register       //  we need this as method name
#define throws(T,...)   noexcept(false) //  we need this to document method signatures

#define failure()       qt_assert("Control failure", __FILE__, __LINE__)
#define failure_with_message(msg)   qt_assert(msg, __FILE__, __LINE__)

//  End of hadesvm-util/Macros.cpp
