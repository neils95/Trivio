namespace TransferDatabase1.NewDb
{
	using System;
	using System.Data.Entity;
	using System.ComponentModel.DataAnnotations.Schema;
	using System.Linq;

	public partial class NewDbContext : DbContext
	{
		public NewDbContext()
			: base("name=NewDbContext")
		{
		}

		public virtual DbSet<C__MigrationHistory> C__MigrationHistory { get; set; }
		public virtual DbSet<Trivia> Trivias { get; set; }
		public virtual DbSet<User> Users { get; set; }
		public virtual DbSet<UserTriviaHistory> UserTriviaHistories { get; set; }

		protected override void OnModelCreating(DbModelBuilder modelBuilder)
		{
		}
	}
}
