#include"Threading.h"

/**
* @brief	�f�X�g���N�^
*/
Threading::~Threading()
{
	Stop();
}

/**
* @brief	Task���s
*
* @reval�@true�@ ����I��
* @reval  false�@�ُ�I��
*/
bool Threading::Start() noexcept
{
	m_isRunning = true;

	// CPU���̍ő�X���b�h�����擾
	// �ő�X���b�h��-1�Ŏg�p(�Ӗ��͓��ɂȂ�)
	int threadCount = std::thread::hardware_concurrency();
	threadCount -= 1;

	// thread��Loop�֐��𐶐� & ���s
	for (int i = 0; i < threadCount; ++i)
	{
		m_threads.emplace_back(&Threading::Loop, this);
	}

	return true;
}

/**
* @brief	TaskStop
*/
void Threading::Stop() noexcept
{
	// ���s����ĂȂ���΁A�I��
	if (!m_isRunning)
	{
		return;
	}

	{
		// �f�[�^�����j�~
		std::unique_lock<std::mutex> lock(m_mutex);
		m_isRunning = false;
	}

	m_conditionVar.notify_all();

	// ���ݎ��s���̏�����҂�
	for (auto& thread : m_threads)
	{
		thread.join();
	}

	m_threads.clear();
}

/**
* @brief	�S�Ẵ^�X�N�I���܂ő҂�
*
* @reval�@true�@ �����I��
*/
bool Threading::WaitUntilTheEnd() noexcept
{
	while (!m_tasks.empty())
	{}
	return true;
}

/**
* @brief	thread�o�^
*
* @param	function [����]�@���s������֐��I�u�W�F�N�g
*/
void Threading::AddTask(ThreadClosure && function) noexcept
{
	// �f�[�^�����j�~
	std::unique_lock<std::mutex> lock(m_mutex);

	// �^�X�N�̒ǉ�
	m_tasks.emplace_back(std::bind(std::forward<ThreadClosure>(function)));

	lock.unlock();

	m_conditionVar.notify_one();
}

/**
* @brief	thread����
*
* @note
*  �e�X���b�h���������s
*  Task�������I��莟��A����Task���Z�b�g���A���s
*/
void Threading::Loop() noexcept
{
	Task task;
	while (true)
	{
		// �f�[�^�����j�~
		std::unique_lock<std::mutex> lock(m_mutex);

		// thread��ҋ@���Anotify_one()���Ă΂��܂ő҂�
		// m_isRunning��false�ɂȂ�܂� return���Ȃ��̂�AddTask����΁A�����ɕ��񏈗������邽��
		m_conditionVar.wait(lock, [this] { return !m_tasks.empty() || !m_isRunning; });
		if (m_tasks.empty() && !m_isRunning)
		{
			return;
		}

		// �擪Task���擾
		task = m_tasks.back();
		m_tasks.pop_back();
		lock.unlock();

		task.Run();
	}
}

/**
* @brief	�R���X�g���N�^
*
* @param	threadClosure [����]�@���s������֐��I�u�W�F�N�g
*/
Threading::Task::Task(Threading::ThreadClosure && threadClosure) noexcept 
	: m_threadClosure(std::forward<Threading::ThreadClosure>(threadClosure))
{}