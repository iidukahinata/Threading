#include"Threading.h"

/**
* @brief	デストラクタ
*/
Threading::~Threading()
{
	Stop();
}

/**
* @brief	Task実行
*
* @reval　true　 正常終了
* @reval  false　異常終了
*/
bool Threading::Start() noexcept
{
	m_isRunning = true;

	// CPU内の最大スレッド数を取得
	// 最大スレッド数-1で使用(意味は特にない)
	int threadCount = std::thread::hardware_concurrency();
	threadCount -= 1;

	// threadにLoop関数を生成 & 実行
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
	// 実行されてなければ、終了
	if (!m_isRunning)
	{
		return;
	}

	{
		// データ競合阻止
		std::unique_lock<std::mutex> lock(m_mutex);
		m_isRunning = false;
	}

	m_conditionVar.notify_all();

	// 現在実行中の処理を待つ
	for (auto& thread : m_threads)
	{
		thread.join();
	}

	m_threads.clear();
}

/**
* @brief	全てのタスク終了まで待つ
*
* @reval　true　 処理終了
*/
bool Threading::WaitUntilTheEnd() noexcept
{
	while (!m_tasks.empty())
	{}
	return true;
}

/**
* @brief	thread登録
*
* @param	function [入力]　実行させる関数オブジェクト
*/
void Threading::AddTask(ThreadClosure && function) noexcept
{
	// データ競合阻止
	std::unique_lock<std::mutex> lock(m_mutex);

	// タスクの追加
	m_tasks.emplace_back(std::bind(std::forward<ThreadClosure>(function)));

	lock.unlock();

	m_conditionVar.notify_one();
}

/**
* @brief	thread処理
*
* @note
*  各スレッド処理を実行
*  Task処理が終わり次第、次のTaskをセットし、実行
*/
void Threading::Loop() noexcept
{
	Task task;
	while (true)
	{
		// データ競合阻止
		std::unique_lock<std::mutex> lock(m_mutex);

		// threadを待機し、notify_one()が呼ばれるまで待つ
		// m_isRunningがfalseになるまで returnしないのはAddTaskすれば、すぐに並列処理させるため
		m_conditionVar.wait(lock, [this] { return !m_tasks.empty() || !m_isRunning; });
		if (m_tasks.empty() && !m_isRunning)
		{
			return;
		}

		// 先頭Taskを取得
		task = m_tasks.back();
		m_tasks.pop_back();
		lock.unlock();

		task.Run();
	}
}

/**
* @brief	コンストラクタ
*
* @param	threadClosure [入力]　実行させる関数オブジェクト
*/
Threading::Task::Task(Threading::ThreadClosure && threadClosure) noexcept 
	: m_threadClosure(std::forward<Threading::ThreadClosure>(threadClosure))
{}